
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)

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
function(target_include_fmt target_name)
    target_include_directories(${target_name} PUBLIC BEFORE ${CMAKE_SOURCE_DIR}/vendor/fmt/include)
endfunction()

#flex
function(target_include_flex target_name)
    target_include_directories(${target_name} PUBLIC BEFORE ${CMAKE_SOURCE_DIR}/vendor/flex/src)
endfunction()

#pybind11
add_subdirectory(pybind11)
#include_directories(pybind11/include)
#include_directories(/usr/include/python3.6)
function(target_include_py_bind target_name)
    target_include_directories(${target_name} PUBLIC BEFORE ${CMAKE_SOURCE_DIR}/vendor/pybind11/include)
    include_directories(/usr/include/python3.6)
endfunction()
