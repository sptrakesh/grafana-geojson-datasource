//
// Created by Rakesh on 02/06/2020.
//

#include "akumuli.h"
#include "log/NanoLog.h"
#include "model/config.h"
#include "util/context.h"
#include "util/split.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/field.hpp>

#include <sstream>
#include <vector>

using spt::model::Response;

namespace spt::client::pakumuli
{
  std::string tagMetric()
  {
    const auto& m = model::Configuration::instance().metric;
    if ( m.empty() )
    {
      LOG_INFO << "Metric not configured.  Cannot process tag-keys or tag-values";
      return {};
    }

    std::string metric;
    if ( m[0] != '!' )
    {
      metric.reserve( m.size() + 1 );
      metric.push_back( '!' );
      metric.append( m );
    }
    else
    {
      metric.reserve( m.size() );
      metric.append( m );
    }

    return metric;
  }

  Response post( const std::string& payload, std::string_view path )
  {
    namespace beast = boost::beast;     // from <boost/beast.hpp>
    namespace http = beast::http;       // from <boost/beast/http.hpp>
    namespace net = boost::asio;        // from <boost/asio.hpp>
    using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

    const auto st = std::chrono::steady_clock::now();
    auto& ch = util::ContextHolder::instance();
    const auto& config = model::Configuration::instance();

    tcp::resolver resolver( ch.ioc );
    beast::tcp_stream stream( ch.ioc );

    auto const results = resolver.resolve( config.akumuli, std::to_string( config.akumuliPort ) );
    stream.connect( results );

    const auto create = [&config, &payload]( beast::string_view location )
    {
      http::request<http::string_body> req{ http::verb::post, location, 11 };
      req.set( http::field::host, config.akumuli );
      req.set( http::field::user_agent, BOOST_BEAST_VERSION_STRING );
      req.keep_alive( true );
      req.set( http::field::content_type, "application/json; charset=utf-8" );
      req.set( http::field::accept, "application/json; charset=utf-8" );
      req.set( http::field::accept_encoding, "gzip" );
      req.body() = payload;
      req.prepare_payload();
      return req;
    };

    http::write( stream, create( beast::string_view{ path.data(), path.size() } ) );

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    beast::error_code ec;
    http::read( stream, buffer, res, ec );
    if (ec)
    {
      LOG_WARN << ec.message();
      return { 500, "", 0 };
    }

    const auto finish = [st, &stream]( http::response<http::string_body>& res ) -> Response
    {
      const auto et = std::chrono::steady_clock::now();
      const auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>( et - st );
      auto result = Response{ int( res.result_int() ),
          { res.body().data(), res.body().size() },
          static_cast<int>( delta.count() ) };

      beast::error_code ec;
      stream.socket().shutdown(tcp::socket::shutdown_both, ec);
      if ( ec && ec != beast::errc::not_connected )
      {
        LOG_INFO << "Error closing tcp socket " << ec.message();
      }

      return result;
    };

    if ( res.result_int() != 301 ) return finish( res );

    auto location = res.find( http::field::location );
    LOG_INFO
        << "Server specified redirect from path: " << path << " to: "
        << location->value().data();
    http::write( stream, create( location->value() ) );

    buffer.clear();
    http::response<http::string_body> res1;
    http::read( stream, buffer, res1 );

    return finish( res1 );
  }
}

Response spt::client::akumuli::query( const spt::model::Query& query )
{
  if ( query.targets.empty() )
  {
    LOG_WARN << "Invalid query without targets";
    return {};
  }

  const auto target = [&query]( int index ) -> Response
  {
    std::string metric;
    if ( query.targets[index].target.empty() )
    {
      LOG_WARN << "Invalid target in query " << index;
      return { 200, "[]", 0 };
    }

    if ( query.targets[index].target[0] != '!' )
    {
      metric.reserve( query.targets[index].target.size() + 1 );
      metric.push_back( '!' );
      metric.append( query.targets[index].target );
    }
    else
    {
      metric.reserve( query.targets[index].target.size() );
      metric.append( query.targets[index].target );
    }

    boost::algorithm::replace_all( metric, "\\", "" );

    std::ostringstream ss;
    ss << '{' <<
       R"("select-events": ")" << metric <<
       R"(", "range": {"from": )" << query.range.fromNs() <<
       ", \"to\": " << query.range.toNs() <<
       R"(}, "output": {"format": "resp", "timestamp": "iso"})" <<
       R"(, "limit": )" << query.maxDataPoints;

    if ( !query.adhocFilters.empty() )
    {
      // TODO: check if same filter key and append multiple values
      ss << R"(, "where": {")" << query.adhocFilters[0].key <<
        "\": \"" << query.adhocFilters[0].value << "\"}";
    }

    ss << '}';
    const auto q = ss.str();
    LOG_DEBUG << "Query for index " << index << ' ' << q;
    auto resp = pakumuli::post( q, "/api/query" );

    if ( resp.status != 200 )
    {
      LOG_WARN << "Event query for index " << index << " rejected with response " << resp.status;
      // Reset to prevent sending error to front-end
      resp.status = 200;
      resp.body = "[]";
      return resp;
    }
    if ( resp.body.empty() || resp.body[0] == '-' )
    {
      LOG_WARN << "Event query for index " << index << " returned error message " << resp.body;
      return resp;
    }

    std::vector<std::string_view> lines = util::split( resp.body, 64, "\r\n" );
    LOG_DEBUG << "Split response into " << int( lines.size() ) << " rows";
    if ( lines.size() < 3 ) return resp;

    auto data = model::LocationResponse{ lines };
    std::ostringstream oss;
    oss << data;
    resp.body = oss.str();
    return resp;
  };

  auto resp = Response{ 200, "[]", 0 };
  std::ostringstream ss;
  ss << '[';

  bool first = true;
  for ( std::size_t i = 0; i < query.targets.size(); ++i )
  {
    if ( !first ) ss << ',';
    const auto r = target( i );
    resp.time += r.time;
    ss << r.body;
    first = false;
  }

  ss << ']';
  resp.body = ss.str();
  return resp;
}

Response spt::client::akumuli::annotations( const spt::model::AnnotationsReq& request )
{
  std::string metric;
  if ( request.annotation.query.empty() )
  {
    LOG_WARN << "Invalid annotation query";
    return {};
  }

  if ( request.annotation.query[0] != '!' )
  {
    metric.reserve( request.annotation.query.size() + 1 );
    metric.push_back( '!' );
    metric.append( request.annotation.query );
  }
  else
  {
    metric.reserve( request.annotation.query.size()  );
    metric.append( request.annotation.query );
  }

  std::ostringstream ss;
  ss << R"({"select-events": ")" << metric <<
     R"(", "range": {"from": )" << request.range.fromNs() <<
     ", \"to\": " << request.range.toNs() <<
     R"(}, "output": {"format": "resp", "timestamp": "iso"})" <<
     R"(, "limit": 1000})";
  const auto q = ss.str();
  LOG_DEBUG << q;
  auto resp = pakumuli::post( q, "/api/query" );

  if ( resp.status != 200 )
  {
    LOG_WARN << "Annotation query rejected with response " << resp.status;
    return resp;
  }
  if ( resp.body.empty() || resp.body[0] == '-' )
  {
    LOG_WARN << "Annotation query returned error message " << resp.body;
    return resp;
  }

  const auto an = model::AnnotationResponse::parse( resp.body );
  std::ostringstream oss;
  oss << '[';

  bool first = true;
  for ( const auto& a : an )
  {
    if ( !first ) oss << ',';
    oss << a;
    first = false;
  }

  oss << ']';
  resp.body = oss.str();

  return resp;
}

Response spt::client::akumuli::search( const spt::model::Target& target )
{
  std::string prefix;
  if ( target.target.empty() ) prefix = "!";
  else if ( target.target[0] != '!' )
  {
    prefix.reserve( target.target.size() + 1 );
    prefix.push_back( '!' );
    prefix.append( target.target );
  }
  else
  {
    prefix.reserve( target.target.size() );
    prefix.append( target.target );
  }

  std::ostringstream ss;
  ss << R"({"select": "metric-names", "starts-with": ")" << prefix << "\"}";
  const auto q = ss.str();
  LOG_DEBUG << q;
  auto resp = pakumuli::post( q, "/api/suggest" );

  if ( resp.status != 200 )
  {
    LOG_WARN << "Series names query rejected with response " << resp.status;
  }
  LOG_DEBUG << "Matching series " << resp.body;

  std::ostringstream oss;
  oss << '[';

  const auto lines = util::split( resp.body, 8, "\r\n" );
  bool first = true;
  for ( const auto line : lines )
  {
    if ( line.size() < 2 ) continue;
    if ( !first ) oss << ',';
    first = false;

    oss << '"';
    if ( line[0] == '+' && line[1] == '!' ) oss << line.substr( 2 );
    else if ( line[0] == '+' ) oss << line.substr( 1 );
    else oss << line;
    oss << '"';
  }

  oss << ']';
  resp.body = oss.str();

  return resp;
}

Response spt::client::akumuli::tagKeys()
{
  const auto metric = pakumuli::tagMetric();
  if ( metric.empty() )
  {
    LOG_INFO << "Metric not configured.  Cannot return tag-keys";
    return { 200, "[]", 0 };
  }

  std::ostringstream ss;
  ss << R"({"select": "tag-names", "starts-with": "", "metric": ")" << metric << "\"}";
  const auto q = ss.str();
  LOG_DEBUG << q;
  auto resp = pakumuli::post( q, "/api/suggest" );

  if ( resp.status != 200 )
  {
    LOG_WARN << "Tag names query rejected with response " << resp.status;
  }
  LOG_DEBUG << "Matching tags " << resp.body;

  std::ostringstream oss;
  oss << '[';

  const auto lines = util::split( resp.body, 8, "\r\n" );
  bool first = true;
  for ( const auto line : lines )
  {
    if ( line.size() < 2 ) continue;

    if ( line.find_first_of( ' ' ) != std::string_view::npos ||
    line.find_first_of( '=' ) != std::string_view::npos )
    {
      LOG_DEBUG << "Skipping line " << line;
      continue;
    }

    if ( !first ) oss << ',';
    oss << R"({"type": "string", "text": ")";
    if ( line[0] == '+' && line[1] == '!' ) oss << line.substr( 2 );
    else if ( line[0] == '+' ) oss << line.substr( 1 );
    else oss << line;
    oss << "\"}";

    first = false;
  }

  oss << ']';
  resp.body = oss.str();
  LOG_DEBUG << "Tag keys " << resp.body;
  return resp;
}

Response spt::client::akumuli::tagValues( const model::Tag& tag )
{
  const auto metric = pakumuli::tagMetric();
  if ( metric.empty() )
  {
    LOG_INFO << "Metric not configured.  Cannot return tag-values";
    return { 200, "[]", 0 };
  }

  std::ostringstream ss;
  ss << R"({"select": "tag-values", "starts-with": ")" << tag.value <<
    R"(", "tag": ")" << tag.key <<
    R"(", "metric": ")" << metric << "\"}";
  const auto q = ss.str();

  LOG_DEBUG << q;
  auto resp = pakumuli::post( q, "/api/suggest" );

  if ( resp.status != 200 )
  {
    LOG_WARN << "Tag values query rejected with response " << resp.status;
  }
  LOG_DEBUG << "Matching tag values " << resp.body;

  std::ostringstream oss;
  oss << '[';

  const auto lines = util::split( resp.body, 8, "\r\n" );
  bool first = true;
  for ( const auto line : lines )
  {
    if ( line.size() < 2 ) continue;
    if ( !first ) oss << ',';
    first = false;

    oss << R"({"type": "string", "text": ")";
    if ( line[0] == '+' && line[1] == '!' ) oss << line.substr( 2 );
    else if ( line[0] == '+' ) oss << line.substr( 1 );
    else oss << line;
    oss << "\"}";
  }

  oss << ']';
  resp.body = oss.str();
  boost::algorithm::replace_all( resp.body, "\\", "" );
  LOG_DEBUG << "Tag values " << resp.body;
  return resp;
}
