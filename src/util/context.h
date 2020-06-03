//
// Created by Rakesh on 03/06/2020.
//

#include <boost/asio/io_context.hpp>

namespace spt::util
{
  struct ContextHolder
  {
    ~ContextHolder() = default;

    ContextHolder( const ContextHolder& ) = delete;
    ContextHolder& operator=( const ContextHolder& ) = delete;

    static ContextHolder& instance( int concurrency = 8 )
    {
      static ContextHolder holder( concurrency );
      return holder;
    }

    boost::asio::io_context ioc;

  private:
    ContextHolder( int concurrency ) : ioc{ concurrency } {}
  };
}
