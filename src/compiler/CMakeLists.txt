
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

#if()
add_definitions(-DYY_NO_UNISTD_H)
#endif()

add_definitions(-D__STDC_VERSION__=199901L)

add_executable(compiler compiler.cpp ${PASCAL_S_COMPILER_SOURCE_FILES})
target_link_libraries(compiler fmt)