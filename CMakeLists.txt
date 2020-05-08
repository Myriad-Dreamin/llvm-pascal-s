
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

message(${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_PROCESSOR} " " ${CMAKE_SYSTEM_VERSION})

set(CMAKE_CXX_STANDARD 17)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_definitions(-DYY_NO_UNISTD_H)
endif ()

# GTest Begin
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(LIBRARIES ${LIBRARIES} " " pthread)
else ()
    message("!!! warning: not using pthread " "(host: " ${CMAKE_SYSTEM_NAME} ")!!!")
endif ()

add_subdirectory(googletest)
# GTest End

#fmt
add_subdirectory(fmt)
include_directories(fmt/include)

#flex
include_directories(flex/src)


# include files
include_directories(include)

# source files
add_subdirectory(src)
