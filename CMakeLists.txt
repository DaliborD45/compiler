cmake_minimum_required(VERSION 3.16)
project(compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(compiler main.cpp
        lexer/lexer.cpp
        lexer/lexer.h
        errors.h
        parser/parser.cpp
        parser/parser.h
        helpers/helpers.cpp
        helpers/helpers.h
        symtable/symtable.cpp
        symtable/symtable.h
        parser/expression_parser/expression_parser.cpp
        parser/expression_parser/expression_parser.h)