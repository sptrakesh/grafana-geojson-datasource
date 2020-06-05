# Grafana Geo:JSON Datasource
Datasource backend for the [Grafana Simple JSON](https://grafana.com/grafana/plugins/grafana-simple-json-datasource)
datasource plugin.

Retrieves events stored in *geo:json* format in [Akumuli](https://akumuli.org/)
in response the end endpoints mandated by the plugin.  We use the datasource to
display location information on Grafana using the [trackmap](https://github.com/alexandrainst/alexandra-trackmap-panel)
panel.

![Site Visitor Map](./resources/sample.png)

## Configuration
The server can be configured with the following options:
* `--port` - The *port* on which the server listens.  Default `8020`.
* `--threads` - Number of I/O threads for the server.  Defaults to `std::thread::hardware_concurrency`.
* `--akumuli-host` - Host name for connecting to *Akumuli*.
* `--akumuli-port` - Port on which *Akumuli* query service listens.  Default
is `8181`.

## Acknowledgements
This software has been developed mainly using work other people have contributed.
The following are the components used to build this software:
* **[Boost:Beast](https://github.com/boostorg/beast)** - We use *Beast* for the
`http` server implementation.  The implementation is a modified version of the
[async](https://github.com/boostorg/beast/tree/develop/example/http/server/async)
example.
* **[Clara](https://github.com/catchorg/Clara)** - Command line options parser.
* **[NanoLog](https://github.com/Iyengar111/NanoLog)** - Logging framework used
for the server.  I modified the implementation for daily rolling log files.
* **[rapidjson](https://github.com/Tencent/rapidjson)** - JSON parser for parsing
request payload and response data from Akumuli.
