
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

llvm_map_components_to_libnames(LLVMLIBS support core irreader)

add_executable(target-llvm_test target_test.cpp)
add_subdirectory(test)