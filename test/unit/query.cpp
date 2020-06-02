//
// Created by Rakesh on 01/06/2020.
//

#include "catch.hpp"
#include "../../src/model/defs.h"

#include <iostream>

SCENARIO("Parse query request")
{
  GIVEN("Sample query from grafana documentation")
  {
    const auto json = R"({
  "panelId": 1,
  "range": {
    "from": "2016-10-31T06:33:44.866Z",
    "to": "2016-10-31T12:33:44.866Z",
    "raw": {
      "from": "now-6h",
      "to": "now"
    }
  },
  "rangeRaw": {
    "from": "now-6h",
    "to": "now"
  },
  "interval": "30s",
  "intervalMs": 30000,
  "targets": [
     { "target": "upper_50", "refId": "A", "type": "timeserie" },
     { "target": "upper_75", "refId": "B", "type": "timeserie" }
  ],
  "adhocFilters": {
    "key": "City",
    "operator": "=",
    "value": "Berlin"
  },
  "format": "json",
  "maxDataPoints": 550
})";

    WHEN("Parsing query")
    {
      auto query = spt::model::Query{};
      auto obj = li::json_object( s::panelId,
          s::range = li::json_object( s::from, s::to, s::raw = li::json_object( s::from, s::to ) ),
          s::rangeRaw = li::json_object( s::from, s::to ),
          s::interval, s::intervalMs,
          s::targets = li::json_vector( s::target, s::refId, s::type ),
          s::adhocFilters = li::json_map<std::string>(),
          s::format, s::maxDataPoints );
      const auto err = obj.decode( json, query );
      if ( !err.what.empty() ) std::cout << err.what << std::endl;
      REQUIRE( err.what.empty() );

      THEN("Query structure populated")
      {
        REQUIRE(query.panelId == 1);
        REQUIRE(query.range.from == "2016-10-31T06:33:44.866Z");
        REQUIRE(query.range.to == "2016-10-31T12:33:44.866Z");
        REQUIRE(query.targets.size() == 2);
        REQUIRE(query.interval == "30s");
        REQUIRE(query.format == "json");
        REQUIRE(query.maxDataPoints == 550);
      }

      THEN("Serialise structure back to JSON")
      {
        const auto js = obj.encode( query );
        std::cout << js << std::endl;
        auto q = spt::model::Query{};
        const auto err = obj.decode( js, q );
        if ( !err.what.empty() ) std::cout << err.what << std::endl;
        REQUIRE( err.what.empty() );
      }
    }
  }

  GIVEN("Partial JSON data can still be parsed")
  {
    const auto json = R"({
  "panelId": 1,
  "range": {
    "from": "2016-10-31T06:33:44.866Z",
    "to": "2016-10-31T12:33:44.866Z",
    "raw": {
      "from": "now-6h",
      "to": "now"
    }
  },
  "rangeRaw": {
    "from": "now-6h",
    "to": "now"
  },
  "interval": "30s",
  "intervalMs": 30000,
  "targets": [
     { "target": "upper_50", "refId": "A", "type": "timeserie" },
     { "target": "upper_75", "refId": "B", "type": "timeserie" }
  ],
  "format": "json",
  "maxDataPoints": 550
})";

    WHEN("Parsing query")
    {
      auto query = spt::model::Query{};
      auto obj = li::json_object( s::panelId,
          s::range = li::json_object( s::from, s::to,
              s::raw = li::json_object( s::from, s::to )),
          s::rangeRaw = li::json_object( s::from, s::to ),
          s::interval, s::intervalMs,
          s::targets = li::json_vector( s::target, s::refId, s::type ),
          s::adhocFilters = li::json_map<std::string>(),
          s::format, s::maxDataPoints );
      const auto err = obj.decode( json, query );
      if ( !err.what.empty()) std::cout << err.what << std::endl;
      REQUIRE_FALSE( err.what.empty());

      THEN( "Query structure populated" )
      {
        REQUIRE( query.panelId == 1 );
        REQUIRE( query.range.from == "2016-10-31T06:33:44.866Z" );
        REQUIRE( query.range.to == "2016-10-31T12:33:44.866Z" );
        REQUIRE( query.targets.size() == 2 );
        REQUIRE( query.interval == "30s" );
        REQUIRE( query.format == "json" );
        REQUIRE( query.maxDataPoints == 550 );
      }

      THEN( "Serialise structure back to JSON" )
      {
        const auto js = obj.encode( query );
        std::cout << js << std::endl;
        auto q = spt::model::Query{};
        const auto err = obj.decode( js, q );
        if ( !err.what.empty()) std::cout << err.what << std::endl;
        REQUIRE_FALSE( err.what.empty());
      }
    }
  }
}

