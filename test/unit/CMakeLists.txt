file(GLOB_RECURSE unit_SRCS *.cpp *.hpp *.h)
set(unit_SRCS ${unit_SRCS})
add_executable(unit ${unit_SRCS} )
target_link_libraries(unit PUBLIC geojson-lib)

