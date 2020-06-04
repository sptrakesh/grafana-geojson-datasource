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

#include <vector>

using spt::model::Response;

namespace spt::client::pakumuli
{
  Response post( const std::string& payload )
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

    const auto create = [&config, &payload]( const std::string& location = "/api/query" )
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

    http::write( stream, create() );

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read( stream, buffer, res );

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
        << "Server specified redirect from path: /api/query to: "
        << location->value().data();
    http::write( stream, create( std::string{ location->value() } ));

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

  std::string metric;
  if ( query.targets[0].target.empty() )
  {
    LOG_WARN << "Invalid initial target in query";
    return {};
  }

  if ( query.targets[0].target[0] != '!' )
  {
    metric.reserve( query.targets[0].target.size() + 1 );
    metric.push_back( '!' );
    metric.append( query.targets[0].target );
  }
  else
  {
    metric.reserve( query.targets[0].target.size() );
    metric.append( query.targets[0].target );
  }

  boost::algorithm::replace_all( metric, "\\", "" );

  std::ostringstream ss;
  ss << '{' <<
    R"("select-events": ")" << metric <<
    R"(", "range": {"from": )" << query.range.fromNs() <<
    ", \"to\": " << query.range.toNs() <<
    R"(}, "output": {"format": "resp", "timestamp": "iso"})" <<
    '}';
  const auto q = ss.str();
  LOG_DEBUG << q;
  auto resp = pakumuli::post( q );

  if ( resp.status != 200 )
  {
    LOG_WARN << "Event query rejected with response " << resp.status;
    return resp;
  }
  if ( resp.body.empty() || resp.body[0] == '-' )
  {
    LOG_WARN << "Event query returned error message " << resp.body;
    return resp;
  }

  LOG_DEBUG << "Events response\n" << resp.body;
  std::vector<std::string_view> lines = util::split( resp.body, 64, "\r\n" );
  if ( lines.size() < 3 ) return resp;

  auto data = model::LocationResponse{};
  data.type = "table";

  data.columns.reserve( 1 );
  data.columns.push_back( model::Column{ "location", "geo:json" } );

  for ( std::size_t i = 2; i < lines.size(); i += 3 )
  {
    LOG_DEBUG << "Parsing line " << lines[i];
    auto row = model::Row{ lines[i] };
    auto v = std::vector<model::Row>{};
    v.reserve( 1 );
    v.push_back( std::move( row ) );
    data.rows.push_back( std::move( v ) );
  }

  return resp;
}

Response spt::client::akumuli::annotations( const spt::model::AnnotationsReq& /* request */ )
{
  return {};
}

Response spt::client::akumuli::search( const spt::model::Query& /* query */ )
{
  return {};
}
