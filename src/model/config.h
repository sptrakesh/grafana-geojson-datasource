//
// Created by Rakesh on 02/06/2020.
//

#pragma once

#include <memory>
#include <string>
#include <thread>

namespace spt::model
{
  struct Configuration
  {
    using Ptr = std::shared_ptr<Configuration>;

    Configuration() = default;
    ~Configuration() = default;
    Configuration( Configuration&& ) = default;
    Configuration& operator=( Configuration&& ) = default;

    Configuration( const Configuration& ) = delete;
    Configuration& operator=( const Configuration& ) = delete;

    std::string akumuli;
    int port = 8020;
    int akumuliPort = 8282;
    int threads = std::thread::hardware_concurrency();

    [[nodiscard]] std::string str() const;
  };
}
