file(GLOB_RECURSE geojson-ds_SRCS *.cpp *.cc *.hpp *.h)
set(geojson-ds_SRCS ${geojson-ds_SRCS})
add_executable(geojson-ds ${geojson-ds_SRCS} )

if (BUILD_TESTING)
  add_library(geojson-lib STATIC ${geojson-ds_SRCS})
endif()

target_link_libraries(geojson-ds
  PRIVATE Boost::boost
)

install(TARGETS geojson-ds DESTINATION bin)
