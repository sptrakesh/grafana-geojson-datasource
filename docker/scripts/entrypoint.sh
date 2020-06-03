#!/bin/sh

LOGDIR=/opt/spt/logs

Defaults()
{
  if [ -z "$PORT" ]
  then
    PORT=8020
    echo "PORT not set.  Will default to $PORT"
  fi

  if [ -z "$THREADS" ]
  then
    THREADS=8
    echo "THREADS not set.  Will default to $THREADS"
  fi
}

Akumuli()
{
  if [ -z "$AKUMULI_HOST" ]
  then
    echo "AKUMULI_HOST not set."
    exit 1
  fi

  if [ -z "$AKUMULI_PORT" ]
  then
    AKUMULI_PORT=8181
    echo "AKUMULI_PORT not set.  Will default to $AKUMULI_PORT"
  fi
}

Service()
{
  if [ ! -d $LOGDIR ]
  then
    mkdir -p $LOGDIR
    chown spt:spt $LOGDIR
  fi

  echo "Starting up Akumuli GeoJSON datasource service"
  /opt/spt/bin/geojson-ds --console true --dir ${LOGDIR}/ \
    --port $PORT --threads $THREADS \
    --akumuli-host "$AKUMULI_HOST" --akumuli-port $AKUMULI_PORT
}

Defaults && Akumuli && Service