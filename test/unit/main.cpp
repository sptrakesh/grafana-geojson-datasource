//
// Created by Rakesh on 01/06/2020.
//

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "../../src/log/NanoLog.h"

int main( int argc, char* argv[] )
{
  nanolog::initialize( nanolog::GuaranteedLogger(), "/tmp", "geojson-test", true );
  return Catch::Session().run( argc, argv );
}

