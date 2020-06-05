#include "server.h"
#include "client/akumuli.h"
#include "log/NanoLog.h"
#include "model/config.h"
#include "util/context.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>

#include <cstdlib>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace spt::server::impl
{
  // Returns a bad request response
  template<class Body, class Allocator>
  auto bad_request( http::request<Body, http::basic_fields<Allocator>>&& req, beast::string_view why )
  {
    http::response<http::string_body> res{http::status::bad_request, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/plain");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  }

  // Returns a not found response
  template<class Body, class Allocator>
  auto not_found( http::request<Body, http::basic_fields<Allocator>>&& req )
  {
    http::response<http::string_body> res{http::status::not_found, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/plain");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string( req.target() ) + "' was not found.";
    res.prepare_payload();
    return res;
  }

  template<class Body, class Allocator>
  auto not_allowed( http::request<Body, http::basic_fields<Allocator>>&& req )
  {
    http::response<http::empty_body> res{ http::status::method_not_allowed,
        req.version() };
    res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
    res.set( http::field::content_type, "text/plain" );
    res.keep_alive( req.keep_alive() );
    res.prepare_payload();
    return res;
  }

  template<class Body, class Allocator>
  auto slash( http::request<Body, http::basic_fields<Allocator>>&& req )
  {
    http::response<http::empty_body> res{http::status::ok, req.version()};
    res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
    res.set( http::field::content_type, "text/plain" );
    res.set( "Access-Control-Allow-Origin", "*" );
    res.set( "Access-Control-Allow-Methods", "GET,POST" );
    res.set( "Access-Control-Allow-Headers", "accept, content-type" );
    res.keep_alive( req.keep_alive() );
    res.prepare_payload();
    return res;
  }

  template<class Body, class Allocator, class Send>
  auto search( http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send )
  {
    auto ct = req[http::field::content_type];
    if ( ct.empty() )
    {
      return send( bad_request( std::move( req ), "Content-Type header not specified" ) );
    }
    if ( !boost::algorithm::starts_with( ct, "application/json" ) )
    {
      return send( bad_request( std::move( req ), "Invalid Content-Type" ) );
    }

    const auto resp = client::akumuli::search( model::Target{ req.body() } );
    http::response<http::string_body> res{http::int_to_status( resp.status ), req.version()};
    res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
    res.set( http::field::content_type, "application/json; charset=utf-8" );
    res.set( "Access-Control-Allow-Origin", "*" );
    res.set( "Access-Control-Allow-Methods", "GET,POST" );
    res.set( "Access-Control-Allow-Headers", "accept, content-type" );
    res.keep_alive( req.keep_alive() );
    res.body() = resp.body;
    res.prepare_payload();
    return send( std::move( res ) );
  }

  template<class Body, class Allocator, class Send>
  auto query( http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send )
  {
    auto ct = req[http::field::content_type];
    if ( ct.empty() )
    {
      return send( bad_request( std::move( req ), "Content-Type header not specified" ) );
    }
    if ( !boost::algorithm::starts_with( ct, "application/json" ) )
    {
      return send( bad_request( std::move( req ), "Invalid Content-Type" ) );
    }

    const auto resp = client::akumuli::query( model::Query{ req.body() } );
    http::response<http::string_body> res{http::int_to_status( resp.status ), req.version()};
    res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
    res.set( http::field::content_type, "application/json; charset=utf-8" );
    res.set( "Access-Control-Allow-Origin", "*" );
    res.set( "Access-Control-Allow-Methods", "GET,POST" );
    res.set( "Access-Control-Allow-Headers", "accept, content-type" );
    res.keep_alive( req.keep_alive() );
    res.body() = resp.body;
    res.prepare_payload();
    return send( std::move( res ) );
  }

  template<class Body, class Allocator, class Send>
  auto annotations( http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send )
  {
    auto ct = req[http::field::content_type];
    if ( ct.empty() )
    {
      return send( bad_request( std::move( req ), "Content-Type header not specified" ) );
    }
    if ( !boost::algorithm::starts_with( ct, "application/json" ) )
    {
      return send( bad_request( std::move( req ), "Invalid Content-Type" ) );
    }

    const auto resp = client::akumuli::annotations( model::AnnotationsReq{ req.body() } );
    http::response<http::string_body> res{http::int_to_status( resp.status ), req.version()};
    res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
    res.set( http::field::content_type, "application/json; charset=utf-8" );
    res.set( "Access-Control-Allow-Origin", "*" );
    res.set( "Access-Control-Allow-Methods", "GET,POST" );
    res.set( "Access-Control-Allow-Headers", "accept, content-type" );
    res.keep_alive( req.keep_alive() );
    res.body() = resp.body;
    res.prepare_payload();
    return send( std::move( res ) );
  }

  template<class Body, class Allocator, class Send>
  auto tag_keys( http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send )
  {
    auto ct = req[http::field::content_type];
    if ( ct.empty() )
    {
      return send( bad_request( std::move( req ), "Content-Type header not specified" ) );
    }
    if ( !boost::algorithm::starts_with( ct, "application/json" ) )
    {
      return send( bad_request( std::move( req ), "Invalid Content-Type" ) );
    }

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
    res.set( http::field::content_type, "application/json; charset=utf-8" );
    res.set( "Access-Control-Allow-Origin", "*" );
    res.set( "Access-Control-Allow-Methods", "GET,POST" );
    res.set( "Access-Control-Allow-Headers", "accept, content-type" );
    res.keep_alive( req.keep_alive() );
    res.body() = "[]";
    res.prepare_payload();
    return send( std::move( res ) );
  }

  template<class Body, class Allocator, class Send>
  auto tag_values( http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send )
  {
    auto ct = req[http::field::content_type];
    if ( ct.empty() )
    {
      return send( bad_request( std::move( req ), "Content-Type header not specified" ) );
    }
    if ( !boost::algorithm::starts_with( ct, "application/json" ) )
    {
      return send( bad_request( std::move( req ), "Invalid Content-Type" ) );
    }

    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
    res.set( http::field::content_type, "application/json; charset=utf-8" );
    res.set( "Access-Control-Allow-Origin", "*" );
    res.set( "Access-Control-Allow-Methods", "GET,POST" );
    res.set( "Access-Control-Allow-Headers", "accept, content-type" );
    res.keep_alive( req.keep_alive() );
    res.body() = "[]";
    res.prepare_payload();
    return send( std::move( res ) );
  }

  // This function produces an HTTP response for the given
  // request. The type of the response object depends on the
  // contents of the request, so the interface requires the
  // caller to pass a generic lambda for receiving the response.
  template<class Body, class Allocator, class Send>
  void handle_request(
      http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send)
  {

    // Make sure we can handle the method
    if( req.method() != http::verb::get &&
        req.method() != http::verb::post &&
        req.method() != http::verb::options )
      return send( not_allowed( std::move( req ) ) );

    // Request path must be absolute and not contain "..".
    if( req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
      return send( bad_request( std::move( req ), "Illegal request-target") );

    if ( req.method() == http::verb::options )
    {
      http::response<http::empty_body> res{ http::status::no_content, req.version() };
      res.set( "Access-Control-Allow-Origin", "*" );
      res.set( "Access-Control-Allow-Methods", "GET,POST" );
      res.set( "Access-Control-Allow-Headers", "accept, content-type" );
      res.set( http::field::server, BOOST_BEAST_VERSION_STRING );
      res.keep_alive( req.keep_alive() );
      return send( std::move( res ) );
    }

    if ( http::verb::get == req.method() && req.target() == "/" )
    {
      return send( slash( std::move( req ) ) );
    }

    if ( http::verb::post == req.method() && req.target() == "/search" )
    {
      return search( std::move( req ), std::forward<Send>( send ) );
    }

    if ( http::verb::post == req.method() && req.target() == "/query" )
    {
      return query( std::move( req ), std::forward<Send>( send ) );
    }

    if ( http::verb::post == req.method() && req.target() == "/annotations" )
    {
      return annotations( std::move( req ), std::forward<Send>( send ) );
    }

    if ( http::verb::post == req.method() && req.target() == "/tag-keys" )
    {
      return tag_keys( std::move( req ), std::forward<Send>( send ) );
    }

    if ( http::verb::post == req.method() && req.target() == "/tag-values" )
    {
      return tag_values( std::move( req ), std::forward<Send>( send ) );
    }

    return send( not_found( std::move( req ) ) );
  }

//------------------------------------------------------------------------------

  // Report a failure
  void fail(beast::error_code ec, char const* what)
  {
    LOG_WARN << what << ": " << ec.message();
  }

// Handles an HTTP server connection
  class session : public std::enable_shared_from_this<session>
  {
    // This is the C++11 equivalent of a generic lambda.
    // The function object is used to send an HTTP message.
    struct send_lambda
    {
      session& self_;

      explicit
      send_lambda(session& self)
          : self_(self)
      {
      }

      template<bool isRequest, class Body, class Fields>
      void
      operator()(http::message<isRequest, Body, Fields>&& msg) const
      {
        // The lifetime of the message has to extend
        // for the duration of the async operation so
        // we use a shared_ptr to manage it.
        auto sp = std::make_shared<
            http::message<isRequest, Body, Fields>>(std::move(msg));

        // Store a type-erased version of the shared
        // pointer in the class to keep it alive.
        self_.res_ = sp;

        // Write the response
        http::async_write(
            self_.stream_,
            *sp,
            beast::bind_front_handler(
                &session::on_write,
                self_.shared_from_this(),
                sp->need_eof()));
      }
    };

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    std::shared_ptr<void> res_;
    send_lambda lambda_;

  public:
    // Take ownership of the stream
    session( tcp::socket&& socket ) : stream_(std::move(socket)), lambda_(*this)
    {
    }

    // Start the asynchronous operation
    void run()
    {
      // We need to be executing within a strand to perform async operations
      // on the I/O objects in this session. Although not strictly necessary
      // for single-threaded contexts, this example code is written to be
      // thread-safe by default.
      net::dispatch(stream_.get_executor(),
          beast::bind_front_handler(
              &session::do_read,
              shared_from_this()));
    }

    void do_read()
    {
      // Make the request empty before reading,
      // otherwise the operation behavior is undefined.
      req_ = {};

      // Set the timeout.
      stream_.expires_after(std::chrono::seconds(30));

      // Read a request
      http::async_read(stream_, buffer_, req_,
          beast::bind_front_handler(
              &session::on_read,
              shared_from_this()));
    }

    void on_read(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      // This means they closed the connection
      if(ec == http::error::end_of_stream) return do_close();

      if(ec) return fail(ec, "read");

      // Send the response
      handle_request( std::move(req_), lambda_ );
    }

    void on_write(
        bool close,
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
      boost::ignore_unused(bytes_transferred);

      if(ec) return fail(ec, "write");

      if(close)
      {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return do_close();
      }

      // We're done with the response so delete it
      res_ = nullptr;

      // Read another request
      do_read();
    }

    void do_close()
    {
      // Send a TCP shutdown
      beast::error_code ec;
      stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

      // At this point the connection is closed gracefully
    }
  };

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
  class listener : public std::enable_shared_from_this<listener>
  {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

  public:
    listener( net::io_context& ioc, tcp::endpoint endpoint)
        : ioc_(ioc), acceptor_(net::make_strand(ioc))
    {
      beast::error_code ec;

      // Open the acceptor
      acceptor_.open(endpoint.protocol(), ec);
      if(ec)
      {
        fail(ec, "open");
        return;
      }

      // Allow address reuse
      acceptor_.set_option(net::socket_base::reuse_address(true), ec);
      if(ec)
      {
        fail(ec, "set_option");
        return;
      }

      // Bind to the server address
      acceptor_.bind(endpoint, ec);
      if(ec)
      {
        fail(ec, "bind");
        return;
      }

      // Start listening for connections
      acceptor_.listen(
          net::socket_base::max_listen_connections, ec);
      if(ec)
      {
        fail(ec, "listen");
        return;
      }
    }

    // Start accepting incoming connections
    void
    run()
    {
      do_accept();
    }

  private:
    void
    do_accept()
    {
      // The new connection gets its own strand
      acceptor_.async_accept(
          net::make_strand(ioc_),
          beast::bind_front_handler(
              &listener::on_accept,
              shared_from_this()));
    }

    void
    on_accept(beast::error_code ec, tcp::socket socket)
    {
      if(ec)
      {
        fail(ec, "accept");
      }
      else
      {
        // Create the session and run it
        std::make_shared<session>( std::move(socket) )->run();
      }

      // Accept another connection
      do_accept();
    }
  };
}

//------------------------------------------------------------------------------

int spt::server::run()
{
  auto const address = net::ip::make_address( "0.0.0.0" );
  const auto& configuration = model::Configuration::instance();
  auto& ch = util::ContextHolder::instance( configuration.threads );

  net::signal_set signals( ch.ioc, SIGINT, SIGTERM );
  signals.async_wait( [&](beast::error_code const&, int) { ch.ioc.stop(); } );

  // Create and launch a listening port
  std::make_shared<impl::listener>( ch.ioc,
      tcp::endpoint{ address, static_cast<unsigned short>( configuration.port ) } )->run();

// Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve( configuration.threads - 1 );
  for( auto i = configuration.threads - 1; i > 0; --i )
  {
    v.emplace_back( [&ch] { ch.ioc.run(); } );
  }

  LOG_INFO << "HTTP service started";
  ch.ioc.run();

  LOG_INFO << "HTTP service stopped";
  for ( auto& t : v ) t.join();
  LOG_INFO << "All I/O threads stopped";

  return EXIT_SUCCESS;
}