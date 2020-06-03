#!/bin/sh

case "$1" in
  'start')
    (cd `dirname $0`;
    docker run -d --rm \
      -v $PWD/dbip-city-lite-2020-06.mmdb.gz:/opt/spt/data/dbip.mmdb.gz:ro \
      -p 8010:8010 \
      --name mmdb-ws mmdb-ws)
    ;;
  'logs')
    docker logs mmdb-ws
    ;;
  'stats')
    docker stats mmdb-ws
    ;;
  'stop')
    docker stop mmdb-ws
    ;;
  *)
    echo "Usage: $0 <start|logs|stats|stop>"
    exit 1
    ;;
esac
