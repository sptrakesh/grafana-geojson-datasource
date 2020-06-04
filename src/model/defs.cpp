//
// Created by Rakesh on 03/06/2020.
//

#include "defs.h"
#include "log/NanoLog.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "util/date.h"

#include <chrono>

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

  int64_t nanoseconds( const std::string& value )
  {
    const auto usi = util::microSeconds( value );
    const auto us = std::chrono::microseconds( usi );
    return std::chrono::duration_cast<std::chrono::nanoseconds>( us ).count();
  }
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
  using rapidjson::Pointer;

  auto d = rapidjson::Document{};
  d.Parse( json.data(), json.size() );
  if ( d.HasParseError() )
  {
    LOG_WARN << "Invalid json specified\n" << json;
    return;
  }

  if ( auto value = Pointer( "/panelId" ).Get( d ) )
  {
    panelId = value->GetInt();
  }
  else LOG_WARN << "Invalid request, missing panelId";

  if ( Pointer( "/range" ).Get( d ) )
  {
    pdefs::parseRange( range, d );
  }
  else LOG_WARN << "Invalid request, missing range";

  if ( auto value = Pointer( "/interval" ).Get( d ) )
  {
    interval = { value->GetString(), value->GetStringLength() };
  }
  else LOG_WARN << "Invalid request, missing interval";

  if ( auto value = Pointer( "/intervalMs" ).Get( d ) )
  {
    intervalMs = value->GetInt();
  }
  else LOG_WARN << "Invalid request, missing intervalMs";

  pdefs::parseTargets( targets, d );
  pdefs::parseAdhocFilters( adhocFilters, d );

  if ( auto value = Pointer( "/format" ).Get( d ) )
  {
    format = { value->GetString(), value->GetStringLength() };
  }
  else LOG_WARN << "Invalid request, missing format";

  if ( auto value = Pointer( "/maxDataPoints" ).Get( d ) )
  {
    maxDataPoints = value->GetInt();
  }
  else LOG_WARN << "Invalid request, missing maxDataPoints";
}
