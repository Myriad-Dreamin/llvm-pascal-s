
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)


function(add_token_test test_name)
    add_gtest(${test_name})
    link_pascal_s_local(${test_name})
endfunction()

add_token_test(token_format_test)
add_token_test(ast_pos_test)