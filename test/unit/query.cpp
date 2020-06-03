//
// Created by Rakesh on 01/06/2020.
//

#include "catch.hpp"
#include "../../src/model/defs.h"

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
  "format": "json",
  "maxDataPoints": 550
})";

    WHEN("Parsing query")
    {
      auto query = spt::model::Query{ json };

      THEN("Query structure populated")
      {
        REQUIRE(query.panelId == 1);
        REQUIRE(query.range.from == "2016-10-31T06:33:44.866Z");
        REQUIRE(query.range.fromNs() == 1477895624866000000);
        REQUIRE(query.range.toNs() == 1477917224866000000);
        REQUIRE(query.targets.size() == 2);
        REQUIRE(query.interval == "30s");
        REQUIRE(query.format == "json");
        REQUIRE(query.maxDataPoints == 550);
      }
    }
  }

  GIVEN("Extra JSON data can still be parsed")
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
  "adhocFilters": [{
    "key": "City",
    "operator": "=",
    "value": "Berlin"
  }],
  "format": "json",
  "maxDataPoints": 550
})";

    WHEN("Parsing query")
    {
      auto query = spt::model::Query{ json };

      THEN( "Query structure populated" )
      {
        REQUIRE( query.panelId == 1 );
        REQUIRE( query.range.from == "2016-10-31T06:33:44.866Z" );
        REQUIRE( query.range.to == "2016-10-31T12:33:44.866Z" );
        REQUIRE(query.range.fromNs() == 1477895624866000000);
        REQUIRE(query.range.toNs() == 1477917224866000000);
        REQUIRE( query.targets.size() == 2 );
        REQUIRE( query.interval == "30s" );
        REQUIRE( query.format == "json" );
        REQUIRE( query.maxDataPoints == 550 );
      }
    }
  }
}

