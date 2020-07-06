
cmake_minimum_required(VERSION 3.15)
project(llvm-pascal-s)


pybind11_add_module(py_pascal_s SHARED py_pascal_s.cpp ${PASCAL_S_RECURSIVE_COMPILER_SOURCE_FILES})
target_link_libraries(py_pascal_s PRIVATE debug fmt)
if (MSVC)
    set_target_properties(py_pascal_s PROPERTIES LINK_FLAGS /MACHINE:X86)
else ()
    set_target_properties(py_pascal_s PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif ()
target_include_flex(py_pascal_s)
target_include_py_bind(py_pascal_s)
