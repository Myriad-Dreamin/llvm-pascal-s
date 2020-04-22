
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

message(${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_PROCESSOR} " " ${CMAKE_SYSTEM_VERSION})

set(CMAKE_CXX_STANDARD 17)

# include files
include_directories(include)

# GTest Begin
if (${CMAKE_SYSTEM_NAME} STREQUAL "LinuxX")
    set(LIBRARIES ${LIBRARIES} " " pthread)
else()
    message("!!! warning: not using pthread " "(host: " ${CMAKE_SYSTEM_NAME} ")!!!")
endif()

add_subdirectory(googletest)
include_directories(googletest/googletest/include)
include_directories(googletest/googlemock/include)
# GTest End


# source files
add_subdirectory(src/lexer)
add_subdirectory(src/parser)
add_subdirectory(src/target-llvm)
add_subdirectory(src/target-c)
