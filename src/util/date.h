//
// Created by Rakesh on 2019-05-29.
//

#pragma once

#include <sstream>

namespace spt::util
{
  bool isLeapYear( int16_t year );
  int64_t microSeconds( const std::string& date );
  int64_t milliSeconds( const std::string& date );
}
