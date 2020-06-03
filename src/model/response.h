//
// Created by Rakesh on 03/06/2020.
//

#pragma once

#include <string>

namespace spt::model
{
  struct Response
  {
    Response() = default;
    ~Response() = default;
    Response( Response&& ) = default;
    Response& operator=( Response&& ) = default;

    Response( const Response& ) = delete;
    Response& operator=( const Response& ) = delete;

    int status;
    std::string body;
    int64_t time;
  };
}
