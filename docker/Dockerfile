FROM sptrakesh/boost as base

COPY CMakeLists.txt /opt/spt/geojson-ds/CMakeLists.txt
COPY src /opt/spt/geojson-ds/src

WORKDIR /opt/spt/geojson-ds/build
RUN rm -rf * \
  && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/opt/local -DBUILD_TESTING=OFF .. \
  && make -j8 \
  && make install

FROM sptrakesh/cppruntime

COPY --from=base /opt/spt/bin /opt/spt/bin

RUN addgroup spt \
  && adduser -DS -g "SPT User" -G spt -s /bin/sh -h /home/spt spt \
  && mkdir -p /opt/spt/logs /opt/spt/data \
  && chown spt:spt /opt/spt/logs /opt/spt/data

COPY docker/scripts/entrypoint.sh /opt/spt/bin

WORKDIR /opt/spt
ENV LD_LIBRARY_PATH=/usr/lib
USER spt
ENTRYPOINT [ "/opt/spt/bin/entrypoint.sh" ]