
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

add_definitions(-DWITH_PASCAL_LEXER_FILES)
add_gtest(target_llvm_test
        ${PASCAL_S_LEXER_SOURCE_FILES}
        ${PASCAL_S_PARSER_SOURCE_FILES}
        ../target-llvm.cpp)
target_link_libraries(target_llvm_test pascal-s-local ${LLVMLIBS})

#remove_definitions(-DWITH_PASCAL_LEXER_FILES)