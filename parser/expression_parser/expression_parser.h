//
// Created by Dalibor Detko on 29/04/2026.
//

#ifndef COMPILER_EXPRESSION_PARSER_H
#define COMPILER_EXPRESSION_PARSER_H
#include <memory>
#include <vector>

#include "../../lexer/lexer.h"



// --- Parser ---
class ExpressionParser {
public:
    Lexer lexer;
    Token currentToken;
    explicit ExpressionParser(const Lexer &lexer): lexer(lexer) {};
    void parseExpression(int min_bp);
private:
    void nud(Token token);
    void parseInfix(Token opToken);
};

#endif //COMPILER_EXPRESSION_PARSER_H
