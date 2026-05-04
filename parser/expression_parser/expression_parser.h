//
// Created by Dalibor Detko on 29/04/2026.
//

#ifndef COMPILER_EXPRESSION_PARSER_H
#define COMPILER_EXPRESSION_PARSER_H
#include "../../lexer/lexer.h"
#include "../abstract_syntax_tree/abstract_syntax_tree.h"


// --- Parser ---
class ExpressionParser {
public:
    // Token currentToken;
    explicit ExpressionParser(Lexer &lexer): lexer(lexer) {};
    Expression parseExpression(int min_bp);
private:
    Lexer& lexer;
    Expression nud(Token token);
    Expression parseInfix(Expression left,Token opToken);
};

#endif //COMPILER_EXPRESSION_PARSER_H
