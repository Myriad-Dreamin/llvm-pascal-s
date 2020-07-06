
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)

function(add_exception_test test_name)
    add_gtest(${test_name})
    target_link_libraries(${test_name} pascal-s-local)
endfunction()

add_exception_test(exception_test)
