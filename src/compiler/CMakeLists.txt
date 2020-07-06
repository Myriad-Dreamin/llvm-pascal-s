
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)

add_definitions(-D__STDC_VERSION__=199901L)


add_definitions(-DWITH_PASCAL_LEXER_FILES)
function(add_compiler compiler-suffix)
    add_executable(nothing-pascal-s${compiler-suffix} ${ARGN})
    target_link_libraries(nothing-pascal-s${compiler-suffix} fmt target-nothing)
    target_include_flex(nothing-pascal-s${compiler-suffix})

    add_executable(llvm-pascal-s${compiler-suffix} ${ARGN})
    target_link_libraries(llvm-pascal-s${compiler-suffix} fmt target-llvm)
    target_include_flex(llvm-pascal-s${compiler-suffix})

    add_definitions(-DPASCAL_S_LLVM_COMPILER_PASS_OPTIMIZE)
    add_executable(optimized-llvm-pascal-s${compiler-suffix} ${ARGN})
    #    target_compile_definitions(optimized-llvm-pascal-s${compiler-suffix} PUBLIC -DPASCAL_S_LLVM_COMPILER_PASS_OPTIMIZE)
    target_link_libraries(optimized-llvm-pascal-s${compiler-suffix} fmt target-llvm)
    target_include_flex(optimized-llvm-pascal-s${compiler-suffix})
endfunction()

add_compiler(-r compiler.cpp ${PASCAL_S_COMPILER_SOURCE_FILES})
add_compiler(-r2 compiler_v2.cpp ${PASCAL_S_COMPILER_SOURCE_FILES})
