
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)


file(GLOB PASCAL_S_TARGET_NOTHING_SOURCE_FILES **.cpp)

add_library(target-nothing ${PASCAL_S_TARGET_NOTHING_SOURCE_FILES})
