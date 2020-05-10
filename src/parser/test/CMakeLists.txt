
cmake_minimum_required(VERSION 3.15)
project(pascal-s)
add_gtest(parser_test ${PASCAL_S_PARSER_SOURCE_FILES})

add_definitions(-DWITH_MOCK)
target_link_libraries(parser_test pascal-s-local)