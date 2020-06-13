//
// Created by Rakesh on 03/06/2020.
//

#include "defs.h"
#include "log/NanoLog.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "util/date.h"
#include "util/split.h"

#include <algorithm>
#include <chrono>
#include <experimental/iterator>
#include <iterator>
#include <sstream>
#include <unordered_map>

#include <boost/algorithm/string/replace.hpp>

namespace spt::model::pdefs
{
  void parseRange( Range& r, rapidjson::Document& d )
  {
    using rapidjson::Pointer;

    if ( auto value = Pointer( "/range/from" ).Get( d ) )
    {
      r.from = { value->GetString(), value->GetStringLength() };
    }
    else LOG_WARN << "Invalid request, missing range.from";

    if ( auto value = Pointer( "/range/to" ).Get( d ) )
    {
      r.to = { value->GetString(), value->GetStringLength() };
    }
    else LOG_WARN << "Invalid request, missing range.to";
  }

  void parseTargets( std::vector<Target>& v, rapidjson::Document& d )
  {
    using rapidjson::Pointer;

    if ( auto value = Pointer( "/targets" ).Get( d ) )
    {
      const auto& a = value->GetArray();
      for ( auto& iter : a )
      {
        Target t;
        const auto& o = iter.GetObject();
        if ( o.HasMember( "target" ) )
        {
          const auto& m = o["target"];
          t.target = { m.GetString(), m.GetStringLength() };
        }

        if ( o.HasMember( "refId" ) )
        {
          const auto& m = o["refId"];
          t.refId = { m.GetString(), m.GetStringLength() };
        }

        if ( o.HasMember( "type" ) )
        {
          const auto& m = o["type"];
          t.type = { m.GetString(), m.GetStringLength() };
        }

        if ( o.HasMember( "data") )
        {
          const auto& da = o["data"].GetObject();
          for ( auto& di : da )
          {
            if ( di.value.IsString() )
            {
              t.data.insert( { { di.name.GetString(), di.name.GetStringLength() },
                                { di.value.GetString(), di.value.GetStringLength() } } );
            }
            else
            {
              LOG_WARN << "Unsupported data with non string value " << di.name.GetString();
            }
          }
        }

        v.push_back( std::move( t ) );
      }
    }
    else LOG_WARN << "Invalid request, missing targets";
  }

  void parseAdhocFilters( std::vector<Filter>& v, rapidjson::Document& d )
  {
    using rapidjson::Pointer;

    if ( auto value = Pointer( "/adhocFilters" ).Get( d ) )
    {
      if ( ! value->IsArray() ) return;

      const auto& a = value->GetArray();
      for ( auto& iter : a )
      {
        Filter f;
        const auto& o = iter.GetObject();
        if ( o.HasMember( "key" ) )
        {
          const auto& m = o["key"];
          f.key = { m.GetString(), m.GetStringLength() };
        }

        if ( o.HasMember( "operator" ) )
        {
          const auto& m = o["operator"];
          f.oper = { m.GetString(), m.GetStringLength() };
        }

        if ( o.HasMember( "value" ) )
        {
          const auto& m = o["value"];
          f.value = { m.GetString(), m.GetStringLength() };
        }

        v.push_back( std::move( f ) );
      }
    }
    else LOG_INFO << "No adhoc filters specified";
  }

  void parseAnnotation( model::Annotation& a, rapidjson::Document& d )
  {
    using rapidjson::Pointer;

    if ( auto value = Pointer( "/annotation/name" ).Get( d ) )
    {
      a.name = { value->GetString(), value->GetStringLength() };
    }
    else LOG_INFO << "Invalid annotation json.  No name specified";

    if ( auto value = Pointer( "/annotation/datasource" ).Get( d ) )
    {
      a.datasource = { value->GetString(), value->GetStringLength() };
    }
    else LOG_INFO << "Invalid annotation json.  No datasource specified";

    if ( auto value = Pointer( "/annotation/enable" ).Get( d ) )
    {
      a.enable = value->GetBool();
    }
    else LOG_INFO << "Invalid annotation json.  No enable specified";

    if ( auto value = Pointer( "/annotation/iconColor" ).Get( d ) )
    {
      a.iconColor = { value->GetString(), value->GetStringLength() };
    }
    else LOG_INFO << "Invalid annotation json.  No iconColor specified";

    if ( auto value = Pointer( "/annotation/query" ).Get( d ) )
    {
      a.query = { value->GetString(), value->GetStringLength() };
    }
    else LOG_INFO << "Invalid annotation json.  No query specified";
  }

  int64_t nanoseconds( const std::string& value )
  {
    const auto usi = util::microSeconds( value );
    const auto us = std::chrono::microseconds( usi );
    return std::chrono::duration_cast<std::chrono::nanoseconds>( us ).count();
  }
}

spt::model::AnnotationsReq::AnnotationsReq( std::string_view json )
{
  LOG_DEBUG << "Parsing annotations request\n" << json;
  using rapidjson::Pointer;

  auto d = rapidjson::Document{};
  d.Parse( json.data(), json.size() );
  if ( d.HasParseError() )
  {
    LOG_WARN << "Invalid annotations request json specified\n" << json;
    return;
  }

  if ( Pointer( "/range" ).Get( d ) )
  {
    pdefs::parseRange( range, d );
  }
  else LOG_WARN << "Invalid annotation request, missing range";

  pdefs::parseAnnotation( annotation, d );
}

std::vector<spt::model::AnnotationResponse> spt::model::AnnotationResponse::parse(
    std::string_view resp )
{
  const auto lines = util::split( resp, 64, "\r\n" );
  std::vector<AnnotationResponse> response;
  response.reserve( lines.size() / 3 );

  for ( std::size_t i = 0; i < lines.size(); ++i )
  {
    const int j = i % 3;

    if ( j == 0 )
    {
      response.push_back( {} );

      auto series = util::split( lines[i], 8, " " );
      for ( std::size_t k = 1; k < series.size(); ++k )
      {
        auto tags = util::split( series[k], 2, "=" );
        if ( tags.size() == 2 )
        {
          auto value = std::string{ tags[1].data(), tags[1].size() };
          boost::algorithm::replace_all( value, "\\", "" );
          response.back().tags.emplace_back(
              Tag{ std::string{ tags[0].data(), tags[0].size() },
              std::move( value ) } );
        }
      }
    }
    else if ( j == 1 )
    {
      response.back().time = util::milliSeconds( { lines[i].data(), lines[i].size() } );
    }
    else
    {
      auto row = model::Row{ lines[i] };
      if ( row.value.coordinates.size() == 2 )
      {
        std::unordered_map<std::string, std::string> map;
        map.reserve( response.back().tags.size() );
        for ( const auto& tag : response.back().tags ) map.insert( { tag.key, tag.value } );

        std::ostringstream  oss;

        auto it = map.find( "continent" );
        if ( it != map.end() ) oss << it->second << ':';
        it = map.find( "country" );
        if ( it != map.end() ) oss << it->second << ':';
        it = map.find( "city" );
        if ( it != map.end() ) oss << it->second << ' ';

        oss << '(' << row.value.coordinates[0] << "," << row.value.coordinates[1] << ')';
        response.back().text = oss.str();
        response.back().title = "Geo-Location";
      }
      else
      {
        response.back().text = "Unparseable location";
      }
    }
  }

  return response;
}

std::ostream& spt::model::operator<<( std::ostream& ss,
    const spt::model::AnnotationResponse& resp )
{
  ss << R"({"text": ")" << resp.text <<
     R"(", "title": ")" << resp.title <<
     R"(", "time": )" << resp.time <<
     R"(, "tags": [)";

  bool first = true;
  for ( const auto& tag : resp.tags )
  {
    if ( !first ) ss << ',';
    ss << '"' << tag.value << '"';
    first = false;
  }

  ss << "]}";
  return ss;
}

spt::model::LocationResponse::LocationResponse( const std::vector<std::string_view>& lines )
{
  load( lines );
}

spt::model::LocationResponse::LocationResponse( std::string_view resp )
{
  std::vector<std::string_view> lines = util::split( resp, 64, "\r\n" );
  load( lines );
}

void spt::model::LocationResponse::load( const std::vector<std::string_view>& lines )
{
  type = "table";

  columns.reserve( 1 );
  columns.push_back( model::Column{ "location", "geo:json" } );

  for ( std::size_t i = 2; i < lines.size(); i += 3 )
  {
    auto row = model::Row{ lines[i] };
    auto v = std::vector<model::Row>{};
    v.reserve( 1 );
    v.push_back( std::move( row ) );
    rows.push_back( std::move( v ) );
  }
}

std::ostream& spt::model::operator<<( std::ostream& ss,
    const spt::model::LocationResponse& resp )
{
  ss << "{\"columns\": [";
  std::copy( std::begin( resp.columns ), std::end( resp.columns ),
      std::experimental::make_ostream_joiner( ss, ", " ) );
  ss << "], \"rows\": [";

  bool vf = true;
  for ( auto& v : resp.rows )
  {
    if ( !vf ) ss << ',';
    ss << '[';
    std::copy( std::begin( v ), std::end( v ),
        std::experimental::make_ostream_joiner( ss, ", " ) );
    ss << ']';
    vf = false;
  }
  ss << R"(], "type": ")" << resp.type << "\"}";
  return ss;
}

std::ostream& spt::model::operator<<( std::ostream& os, const spt::model::Column& column )
{
  os << R"({"text": ")" << column.text << R"(", "type": ")" << column.type << "\"}";
  return os;
}

std::ostream& spt::model::operator<<( std::ostream& os, const spt::model::Row& row )
{
  os << R"({"type": ")" << row.type << R"(", "value": {"type": ")" <<
     row.value.type << R"(", "coordinates": [)" <<
     row.value.coordinates[0] << ',' << row.value.coordinates[1] <<
     R"(], "metadata": {"timestamp": {"type": ")" << row.metadata.timestamp.type <<
     R"(", "value": ")" << row.metadata.timestamp.value << "\"}}}}";
  return os;
}

int64_t spt::model::Timestamp::valueNs() const
{
  return pdefs::nanoseconds( value );
}

spt::model::Row::Row( std::string_view json )
{
  using rapidjson::Pointer;

  if ( json.empty() || json.size() < 2 )
  {
    LOG_WARN << "Empty JSON specified";
    return;
  }

  // Trim initial + char
  auto trimmed = json[0] == '+' ? json.substr( 1 ) : json;

  auto d = rapidjson::Document{};
  d.Parse( trimmed.data(), trimmed.size() );
  if ( d.HasParseError() )
  {
    LOG_WARN << "Invalid json specified\n" << json;
    return;
  }

  if ( auto v = Pointer( "/type" ).Get( d ) )
  {
    type = { v->GetString(), v->GetStringLength() };
  }
  else LOG_WARN << "Invalid row, missing type";

  if ( auto v = Pointer( "/value/type" ).Get( d ) )
  {
    value.type = { v->GetString(), v->GetStringLength() };
  }
  else LOG_WARN << "Invalid row, missing value.type";

  if ( auto v = Pointer( "/value/coordinates" ).Get( d ) )
  {
    const auto arr = v->GetArray();
    if ( arr.Size() < 2 )
    {
      LOG_WARN << "Invalid coordinates array of size " << arr.Size();
      return;
    }

    value.coordinates.reserve( 2 );
    value.coordinates.push_back( arr[0].GetDouble() );
    value.coordinates.push_back( arr[1].GetDouble() );
  }
  else LOG_WARN << "Invalid row, missing value.coordinates";

  if ( auto v = Pointer( "/metadata/timestamp/type" ).Get( d ) )
  {
    metadata.timestamp.type = { v->GetString(), v->GetStringLength() };
  }
  else LOG_WARN << "Invalid row, missing metadata.timestamp.type";

  if ( auto v = Pointer( "/metadata/timestamp/value" ).Get( d ) )
  {
    metadata.timestamp.value = { v->GetString(), v->GetStringLength() };
  }
  else LOG_WARN << "Invalid row, missing metadata.timestamp.value";
}

int64_t spt::model::Range::fromNs() const
{
  return pdefs::nanoseconds( from );
}

int64_t spt::model::Range::toNs() const
{
  return pdefs::nanoseconds( to );
}

spt::model::Query::Query( std::string_view json )
{
  LOG_DEBUG << "Parsing query\n" << json;
  using rapidjson::Pointer;

  auto d = rapidjson::Document{};
  d.Parse( json.data(), json.size() );
  if ( d.HasParseError() )
  {
    LOG_WARN << "Invalid query json specified\n" << json;
    return;
  }

  if ( auto value = Pointer( "/panelId" ).Get( d ) )
  {
    if ( value->IsInt64() ) panelId = value->GetInt64();
    else if ( value->IsInt() ) panelId = value->GetInt();
  }
  else LOG_WARN << "Invalid query request, missing panelId";

  if ( Pointer( "/range" ).Get( d ) )
  {
    pdefs::parseRange( range, d );
  }
  else LOG_WARN << "Invalid query request, missing range";

  if ( auto value = Pointer( "/interval" ).Get( d ) )
  {
    interval = { value->GetString(), value->GetStringLength() };
  }
  else LOG_WARN << "Invalid query request, missing interval";

  if ( auto value = Pointer( "/intervalMs" ).Get( d ) )
  {
    intervalMs = value->GetInt();
  }
  else LOG_WARN << "Invalid query request, missing intervalMs";

  pdefs::parseTargets( targets, d );
  pdefs::parseAdhocFilters( adhocFilters, d );

  if ( auto value = Pointer( "/format" ).Get( d ) )
  {
    format = { value->GetString(), value->GetStringLength() };
  }
  else LOG_WARN << "Invalid query request, missing format";

  if ( auto value = Pointer( "/maxDataPoints" ).Get( d ) )
  {
    maxDataPoints = value->GetInt();
  }
  else LOG_WARN << "Invalid query request, missing maxDataPoints";
}

void spt::model::Tag::parse( std::string_view json )
{
  LOG_DEBUG << "Parsing tag " << json;
  using rapidjson::Pointer;

  auto d = rapidjson::Document{};
  d.Parse( json.data(), json.size() );
  if ( d.HasParseError() )
  {
    LOG_WARN << "Invalid tag specified\n" << json;
    return;
  }

  if ( auto v = Pointer( "/key" ).Get( d ) )
  {
    key = { v->GetString(), v->GetStringLength() };
  }
  else LOG_WARN << "Invalid tag request, missing key";

  if ( auto v = Pointer( "/value" ).Get( d ) )
  {
    value = { v->GetString(), v->GetStringLength() };
  }
  else LOG_INFO << "No tag value";
}

spt::model::Target::Target( std::string_view json )
{
  LOG_DEBUG << "Parsing target\n" << json;
  using rapidjson::Pointer;

  auto d = rapidjson::Document{};
  d.Parse( json.data(), json.size() );
  if ( d.HasParseError() )
  {
    LOG_WARN << "Invalid target json specified\n" << json;
    return;
  }

  if ( auto value = Pointer( "/target" ).Get( d ) )
  {
    target = { value->GetString(), value->GetStringLength() };
  }
  else LOG_WARN << "Invalid target request, missing target";

  if ( auto value = Pointer( "/refId" ).Get( d ) )
  {
    refId = { value->GetString(), value->GetStringLength() };
  }
  else LOG_WARN << "Invalid target request, missing refId";

  if ( auto value = Pointer( "/type" ).Get( d ) )
  {
    type = { value->GetString(), value->GetStringLength() };
  }
  else LOG_WARN << "Invalid target request, missing type";
}
