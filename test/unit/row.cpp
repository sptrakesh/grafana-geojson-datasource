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

/*
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod2 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200513T173909.079000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.116,-88.0545]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-13T17:39:09.079000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod3 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200513T174754.198000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1162,-88.0546]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-13T17:47:54.198000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200513T175555.651000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.116,-88.0542]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-13T17:55:55.651000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200513T180109.015000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1163,-88.0542]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-13T18:01:09.015000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T195601.739000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1161,-88.0543]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T19:56:01.739000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod3 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T195853.644000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1161,-88.0544]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T19:58:53.644000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod3 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T201515.485000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1154,-88.0538]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T20:15:15.485000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T202534.691000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.116,-88.0541]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T20:25:34.691000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T202711.171000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1161,-88.0541]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T20:27:11.171000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod2 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T203022.607000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1161,-88.0537]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T20:30:22.607000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod2 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T212025.726000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1161,-88.0554]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T21:20:25.726000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T213114.853000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1162,-88.0541]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T21:31:14.853000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod2 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200515T213345.646000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1159,-88.0546]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-15T21:33:45.646000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200519T131330.259000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [39.2873,-76.6124]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-19T13:13:30.259000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod2 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200519T135439.091000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [39.288,-76.613]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-19T13:54:39.091000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod3 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200519T143613.006000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [42.1161,-88.0543]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-19T14:36:13.006000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod1 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200519T224808.377000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [39.2705,-76.62]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-19T22:48:08.377000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod2 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200519T230633.106000000
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [39.2705,-76.6203]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-19T23:06:33.106000Z"}}}
+!sensor.location customerCode=wirepulse customerOid=5cf144b483824a0f6857e7c4 host=prod3 sensorIdentifier=54E054307E39C054 sensorOid=5ebc2f78580716546574e647 topic=/location
+20200519T232800.613000001
+{"type": "geo:json", "value": {"type": "Point", "coordinates": [39.2726,-76.6223]}, "metadata": {"timestamp": {"type": "DateTime", "value": "2020-05-19T23:28:00.057000Z"}}}
 */

