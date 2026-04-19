#include <iostream>
#include <string>
#include <fstream>
#include "lexer/lexer.h"
#include "parser/parser.h"

std::string FILE_NAME = "../program.dd";

/**
 * Syntax driven translation, so the main point is syntax analyser,
 */
int main() {
    std::ifstream file(FILE_NAME);
    if (!file.is_open()) {
        std::cout << "cannot open file" << std::endl;
    }
    Token token;
    const Lexer lexer(file);
    Parser parser(lexer);
    try {
        parser.createTree();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "program has parsed successfully" << std::endl;
    return 0;
}