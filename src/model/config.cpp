//
// Created by Rakesh on 02/06/2020.
//

#include "config.h"

#include <sstream>

std::string spt::model::Configuration::str() const
{
  std::ostringstream ss;
  ss << "{\"port\": " << port <<
     ", \"threads\": " << threads <<
     R"(, "akumuli": {"host": ")" << akumuli <<
     R"(", "port": )" << akumuliPort <<
     R"(, "metric": ")" << metric <<
     "\"}}";
  return ss.str();
}

