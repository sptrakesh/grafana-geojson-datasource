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
}

int64_t spt::model::Range::fromNs() const
{
  const auto usi = util::microSeconds( from );
  const auto us = std::chrono::microseconds( usi );
  return std::chrono::duration_cast<std::chrono::nanoseconds>( us ).count();
}

int64_t spt::model::Range::toNs() const
{
  const auto usi = util::microSeconds( to );
  const auto us = std::chrono::microseconds( usi );
  return std::chrono::duration_cast<std::chrono::nanoseconds>( us ).count();
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
