#!/bin/sh

case "$1" in
  'start')
    (cd `dirname $0`;
    docker run -d --rm \
      -p 8020:8020 \
      --name geojson-ds geojson-ds)
    ;;
  'logs')
    docker logs geojson-ds
    ;;
  'stats')
    docker stats geojson-ds
    ;;
  'stop')
    docker stop geojson-ds
    ;;
  *)
    echo "Usage: $0 <start|logs|stats|stop>"
    exit 1
    ;;
esac
