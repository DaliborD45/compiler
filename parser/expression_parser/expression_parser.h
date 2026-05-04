//
// Created by Dalibor Detko on 29/04/2026.
//

#ifndef COMPILER_EXPRESSION_PARSER_H
#define COMPILER_EXPRESSION_PARSER_H
#include "../../lexer/lexer.h"



// --- Parser ---
class ExpressionParser {
public:
    // Token currentToken;
    explicit ExpressionParser(Lexer &lexer): lexer(lexer) {};
    void parseExpression(int min_bp);
private:
    Lexer& lexer;
    void nud(Token token);
    void parseInfix(Token opToken);
};

#endif //COMPILER_EXPRESSION_PARSER_H
