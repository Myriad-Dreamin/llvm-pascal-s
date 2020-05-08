
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

# test_main source files -> compile to lib to reduce compile time
add_library(gtest-main STATIC test/test-main.cpp)
target_link_libraries(gtest-main gtest gmock)
target_include_directories(gtest-main SYSTEM PUBLIC gtest gmock fmt)

# description: create a gtest target
# param:
#     test_name: string, the name of target (also the name of entry file)
# example:
#     if there is a test source file named my_test
#         my_test.cpp (without main function, which provided by test_main.cpp)
#     then add following target to CMakeList.txt
#     add_gtest(my_test)
function(add_gtest test_name)
    add_executable(${test_name} ${test_name}.cpp ${ARGN})
    target_link_libraries(${test_name} gtest-main fmt)
    if (MINGW)
        target_link_libraries(${test_name} -static-libgcc -static-libstdc++)
    endif ()
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# token/exception source files -> compile to lib to reduce compile time
file(GLOB PASCAL_S_LOCAL_LIBRARY_SOURCE_FILES exception/*.cpp exception/*.cc token-ast/*.cpp token-ast/*.cc)
add_library(pascal-s-local ${PASCAL_S_LOCAL_LIBRARY_SOURCE_FILES})
target_link_libraries(pascal-s-local fmt)
function(link_pascal_s_local target)
    target_link_libraries(${target} pascal-s-local)
endfunction()
add_subdirectory(token-ast/test)
add_subdirectory(exception/test)

# lexer source files -> compile to lib to reduce compile time
file(GLOB PASCAL_S_LEXER_SOURCE_FILES lexer/*.cpp lexer/*.cc)
add_library(pascal-s-lexer ${PASCAL_S_LEXER_SOURCE_FILES})
link_pascal_s_local(pascal-s-lexer)
function(link_pascal_s_lexer target)
    target_link_libraries(${target} pascal-s-lexer)
endfunction()
add_subdirectory(lexer/test)

# parser source files
file(GLOB PASCAL_S_PARSER_SOURCE_FILES parser/*.cpp parser/*.cc)
add_subdirectory(parser/test)

# other pascal-s compiler layers
add_subdirectory(target-llvm)
add_subdirectory(target-c)

# compiler source files
file(GLOB PASCAL_S_COMPILER_FEATURES_SOURCE_FILES compiler-features/*.cpp compiler-features/*.cc)
list(APPEND PASCAL_S_COMPILER_SOURCE_FILES
        ${PASCAL_S_LOCAL_LIBRARY_SOURCE_FILES}
        ${PASCAL_S_LEXER_SOURCE_FILES}
        ${PASCAL_S_COMPILER_FEATURES_SOURCE_FILES}
        )
add_subdirectory(compiler)

add_subdirectory(test)