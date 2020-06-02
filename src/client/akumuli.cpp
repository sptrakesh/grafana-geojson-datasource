//
// Created by Rakesh on 02/06/2020.
//

#include "akumuli.h"
#include "log/NanoLog.h"

#include <chrono>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http/impl/verb.ipp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/field.hpp>

namespace spt::client::pakumuli
{
  std::string post( const model::Configuration* configuration, std::string&& payload )
  {
    namespace beast = boost::beast;     // from <boost/beast.hpp>
    namespace http = beast::http;       // from <boost/beast/http.hpp>
    namespace net = boost::asio;        // from <boost/asio.hpp>
    using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

    const auto st = std::chrono::steady_clock::now();

    return {};
  }
}

std::string spt::client::akumuli::query(
    const model::Configuration* configuration, const spt::model::Query& query )
{
  return {};
}

std::string spt::client::akumuli::annotations(
    const model::Configuration* configuration, const spt::model::AnnotationsReq& request )
{
  return {};
}

std::string spt::client::akumuli::search(
    const model::Configuration* configuration, const spt::model::Query& query )
{
  return {};
}
