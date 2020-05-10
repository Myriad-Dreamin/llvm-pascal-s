
cmake_minimum_required(VERSION 3.15)
project(pascal-s)

add_gtest(target_llvm_test ${PASCAL_S_LEXER_SOURCE_FILES} ${PASCAL_S_PARSER_SOURCE_FILES} ${PASCAL_S_TARGET_LLVM_SOURCE_FILES})
target_link_libraries(target_llvm_test pascal-s-local ${LLVMLIBS})
