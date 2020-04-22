
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

add_executable(lexer_test lexer_test.cpp)

add_executable(token_test token_test.cpp)
target_link_libraries(token_test gtest)
