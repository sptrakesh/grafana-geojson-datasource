//
// Created by Rakesh on 03/06/2020.
//

#include "catch.hpp"
#include "../../src/model/defs.h"

SCENARIO( "Parse Row response from Akumuli" )
{
  GIVEN( "A sample row in geojson format" )
  {
    const auto json = R"({
  "type": "geo:json",
  "value": {
    "type": "Point",
    "coordinates": [39.2726, -76.6223]
  },
  "metadata": {
    "timestamp": {
      "type": "DateTime",
      "value": "2020-05-19T23:28:00.057000Z"
    }
  }
})";

    WHEN( "Parsing data into row model" )
    {
      auto row = spt::model::Row{ json };

      THEN( "All information parsed into corresponding fields" )
      {
        REQUIRE( row.type == "geo:json" );
        REQUIRE( row.value.type == "Point" );
        REQUIRE( row.value.coordinates.size() == 2 );
        REQUIRE( row.value.coordinates[0] == 39.2726 );
        REQUIRE( row.value.coordinates[1] == -76.6223 );
        REQUIRE( row.metadata.timestamp.type == "DateTime" );
        REQUIRE( row.metadata.timestamp.value == "2020-05-19T23:28:00.057000Z" );
        REQUIRE( row.metadata.timestamp.valueNs() == 1589930880057000000 );
      }
    }
  }

  GIVEN( "A sample row response line in geojson format" )
  {
    const auto json = R"(+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.116,-88.0545]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-13T17:39:09.079000Z"}}})";

    WHEN( "Parsing data into row model" )
    {
      auto row = spt::model::Row{ json };

      THEN( "All information parsed into corresponding fields" )
      {
        REQUIRE( row.type == "geo:json" );
        REQUIRE( row.value.type == "Point" );
        REQUIRE( row.value.coordinates.size() == 2 );
        REQUIRE( row.value.coordinates[0] == 42.116 );
        REQUIRE( row.value.coordinates[1] == -88.0545 );
        REQUIRE( row.metadata.timestamp.type == "DateTime" );
        REQUIRE( row.metadata.timestamp.value == "2020-05-13T17:39:09.079000Z" );
        REQUIRE( row.metadata.timestamp.valueNs() == 1589391549079000000 );
      }
    }
  }
}


