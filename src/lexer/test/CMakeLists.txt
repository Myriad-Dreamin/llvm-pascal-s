
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

add_executable(lexer_test lexer_test.cpp)
target_link_libraries(lexer_test pascal-s-lexer pascal-s-local fmt)

function(add_lexer_test test_name)
    add_gtest(${test_name})
    target_link_libraries(${test_name} pascal-s-local pascal-s-lexer)
endfunction()

add_lexer_test(token_test)
#add_gtest(lexer_interface_test)