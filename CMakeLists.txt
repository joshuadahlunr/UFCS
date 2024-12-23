cmake_minimum_required(VERSION 3.20)
project(UFCS CXX C)

add_subdirectory(argparse)
add_subdirectory(tree-sitter/lib)

add_library(tree-sitter-ufcsc STATIC "tree-sitter-ufcsc/src/parser.c" "tree-sitter-ufcsc/src/scanner.c")
target_include_directories(tree-sitter-ufcsc PUBLIC tree-sitter-ufcsc/bindings/c)

add_executable(ufcs ufcs.cpp)
target_link_libraries(ufcs PUBLIC tree-sitter tree-sitter-ufcsc argparse)