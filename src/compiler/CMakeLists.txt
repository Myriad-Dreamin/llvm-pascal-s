
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

add_definitions(-D__STDC_VERSION__=199901L)

add_executable(compiler compiler.cpp ${PASCAL_S_COMPILER_SOURCE_FILES})

target_link_libraries(compiler fmt debug target-llvm)
