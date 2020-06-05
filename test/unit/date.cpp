//
// Created by Rakesh on 05/06/2020.
//

#include "catch.hpp"
#include "../../src/util/date.h"

SCENARIO( "Parse simple ISO format date from Akumuli" )
{
  GIVEN( "A sample nanosecond timestamp from Akumuli" )
  {
    const auto ts = "20200513T180109.015000000";

    WHEN( "Parsing into milliseconds" )
    {
      auto epoch = spt::util::milliSeconds( ts );
      REQUIRE( epoch == 1589392869015 );
    }
  }

  GIVEN( "Another sample nanosecond timestamp from Akumuli" )
  {
    const auto ts = "20200515T195853.644000000";

    WHEN( "Parsing into milliseconds" )
    {
      auto epoch = spt::util::milliSeconds( ts );
      REQUIRE( epoch == 1589572733644 );
    }
  }
}

