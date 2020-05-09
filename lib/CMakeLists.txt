
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

set(OBJS ${CMAKE_CURRENT_SOURCE_DIR}/main.o)

add_executable(pascal-s-run libstdps/main.cpp ${OBJS})

set_source_files_properties(
        ${OBJS}
        PROPERTIES
        EXTERNAL_OBJECT true
        GENERATED true
)