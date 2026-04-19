cmake_minimum_required(VERSION 4.2)
project(compiler)

add_compile_options(-std=c++17)
set(CMAKE_CXX_STANDARD 17)

add_executable(compiler main.cpp
        lexer/lexer.cpp
        lexer/lexer.h
#        tests/lexer/tests.cpp
#        tests/lexer/tests.h
        errors.h
        parser/parser.cpp
        parser/parser.h
        helpers/helpers.cpp
        helpers/helpers.h
        symtable/symtable.cpp
        symtable/symtable.h)
