
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

file(GLOB LIBSTDPS_SOURCE_FILES libstdps/*.cpp)

add_library(libstdps ${LIBSTDPS_SOURCE_FILES})


# test
set(OBJS ${CMAKE_CURRENT_SOURCE_DIR}/main.o)
add_executable(pascal-s-run ${LIBSTDPS_SOURCE_FILES} ${OBJS})

set_source_files_properties(
        ${OBJS}
        PROPERTIES
        EXTERNAL_OBJECT true
        GENERATED true
)