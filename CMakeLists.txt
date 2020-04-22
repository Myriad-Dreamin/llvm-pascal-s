
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

message(${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_PROCESSOR} " " ${CMAKE_SYSTEM_VERSION})

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_subdirectory(src/lexer)
add_subdirectory(src/parser)
add_subdirectory(src/target-llvm)
add_subdirectory(src/target-c)
