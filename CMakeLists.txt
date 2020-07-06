
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)

#set(PYTHON_EXECUTABLE "C:\\Users\\kamiyoru\\AppData\\Local\\Programs\\Python\\Python38-32\\python.exe")

message(${CMAKE_SYSTEM_NAME} " " ${CMAKE_SYSTEM_PROCESSOR} " " ${CMAKE_SYSTEM_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_definitions(-DYY_NO_UNISTD_H)
endif ()

option(ENABLE_LLVM "enable llvm related targets"
        OFF)
set(LLVM_PATH "C:/work/cpp/llvm-tutorial/lib/build-llvm/lib/cmake/llvm" CACHE STRING "path to llvm lib")

# include files (header, configuration, and others can include)
include_directories(include)
include_directories(vendor)

# vendor files (external libraries)
add_subdirectory(vendor)

if (ENABLE_LLVM)
    set(LLVM_DIR ${LLVM_PATH})
    find_package(LLVM REQUIRED CONFIG)

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    message(STATUS "llvm include dir ${LLVM_INCLUDE_DIRS}")

    add_definitions(${LLVM_DEFINITIONS})
    include_directories(${LLVM_INCLUDE_DIRS})
    #set_property(GLOBAL PROPERTY _target_llvm_include_directory_property ${LLVM_INCLUDE_DIRS})
endif ()

# lib files (internal libraries)
add_subdirectory(lib)

# source files (implementations)
add_subdirectory(src)
