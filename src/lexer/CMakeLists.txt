
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test fmt)

add_gtest(token_test)
#add_gtest(lexer_interface_test)