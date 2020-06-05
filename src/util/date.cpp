//
// Created by Rakesh on 19/12/2019.
//

#include "date.h"

#include <chrono>

bool spt::util::isLeapYear( int16_t year )
{
  bool result = false;

  if ( ( year % 400 ) == 0 ) result = true;
  else if ( ( year % 100 ) == 0 ) result = false;
  else if ( ( year % 4 ) == 0 ) result = true;

  return result;
}

int64_t spt::util::microSeconds( const std::string& date )
{
  const auto microSecondsPerHour = int64_t( 3600000000 );

  char *end;
  const int16_t year = std::strtol( date.substr( 0, 4 ).c_str(), &end, 10 );
  const int16_t month = std::strtol( date.substr( 5, 2 ).c_str(), &end, 10 );
  const int16_t day = std::strtol( date.substr( 8, 2 ).c_str(), &end, 10 );
  const int16_t hour = std::strtol( date.substr( 11, 2 ).c_str(), &end, 10 );
  const int16_t minute = std::strtol( date.substr( 14, 2 ).c_str(), &end, 10 );
  const int16_t second = std::strtol( date.substr( 17, 2 ).c_str(), &end, 10 );
  const int16_t millis = std::strtol( date.substr( 20, 3 ).c_str(), &end, 10 );
  const int16_t micros = std::strtol( date.substr( 23, 3 ).c_str(), &end, 10 );

  int64_t epoch = micros;
  epoch += millis * int64_t( 1000 );
  epoch += second * int64_t( 1000000 );
  epoch += minute * int64_t( 60000000 );
  epoch += hour * microSecondsPerHour;
  epoch += ( day - 1 ) * 24 * microSecondsPerHour;

  const int8_t isLeap = isLeapYear( year );

  for ( int i = 1; i < month; ++i )
  {
    switch ( i )
    {
    case 2:
      epoch += ( (isLeap) ? 29 : 28 ) * 24 * microSecondsPerHour;
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      epoch += 30 * 24 * microSecondsPerHour;
      break;
    default:
      epoch += 31 * 24 * microSecondsPerHour;
    }
  }

  for ( int i = 1970; i < year; ++i )
  {
    if ( isLeapYear( i ) ) epoch += 366 * 24 * microSecondsPerHour;
    else epoch += 365 * 24 * microSecondsPerHour;
  }

  return epoch;
}

int64_t spt::util::milliSeconds( const std::string& date )
{
  const auto microSecondsPerHour = int64_t( 3600000000 );

  char *end;
  const int16_t year = std::strtol( date.substr( 0, 4 ).c_str(), &end, 10 );
  const int16_t month = std::strtol( date.substr( 4, 2 ).c_str(), &end, 10 );
  const int16_t day = std::strtol( date.substr( 6, 2 ).c_str(), &end, 10 );
  const int16_t hour = std::strtol( date.substr( 9, 2 ).c_str(), &end, 10 );
  const int16_t minute = std::strtol( date.substr( 11, 2 ).c_str(), &end, 10 );
  const int16_t second = std::strtol( date.substr( 13, 2 ).c_str(), &end, 10 );
  const int16_t millis = std::strtol( date.substr( 16, 3 ).c_str(), &end, 10 );
  const int16_t micros = std::strtol( date.substr( 19, 3 ).c_str(), &end, 10 );

  int64_t epoch = micros;
  epoch += millis * int64_t( 1000 );
  epoch += second * int64_t( 1000000 );
  epoch += minute * int64_t( 60000000 );
  epoch += hour * microSecondsPerHour;
  epoch += ( day - 1 ) * 24 * microSecondsPerHour;

  const int8_t isLeap = isLeapYear( year );

  for ( int i = 1; i < month; ++i )
  {
    switch ( i )
    {
    case 2:
      epoch += ( (isLeap) ? 29 : 28 ) * 24 * microSecondsPerHour;
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      epoch += 30 * 24 * microSecondsPerHour;
      break;
    default:
      epoch += 31 * 24 * microSecondsPerHour;
    }
  }

  for ( int i = 1970; i < year; ++i )
  {
    if ( isLeapYear( i ) ) epoch += 366 * 24 * microSecondsPerHour;
    else epoch += 365 * 24 * microSecondsPerHour;
  }

  auto us = std::chrono::microseconds{ epoch };
  return std::chrono::duration_cast<std::chrono::milliseconds>( us ).count();
}
