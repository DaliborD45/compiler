//
// Created by Dalibor Detko on 29/04/2026.
//

#include "expression_parser.h"
#include "../../parser/parser.h"
#include "vector"
#include "../../helpers/helpers.h"

/**
 * INFIX: operator is between operands (*, /, ., (,)
 * PREFIX: operator is in front of operand (-5, +4)
 */


int getPrecedence(const TokenTypeEnum type) {
    switch (type) {
        case DOT:
        case LEFT_PAREN: return 40;

        case STAR:
        case SLASH:  return 20;

        case PLUS:
        case MINUS:  return 10;

        case GREATER_THAN_SIGN:
        case LESS_THAN_SIGN:
        case GREATER_EQ_THAN_SIGN:
        case LESS_EQ_THAN_SIGN:
        case DOUBLE_EQ:
        case NOT_EQ: return 5;

        default:     return 0;
    }
}
std::vector<TokenTypeEnum> operators = {PLUS, MINUS, STAR, SLASH};


void ExpressionParser::nud(Token token) {
    switch (token.type) {
        case NUMBER:
        case STRING:
        case F64_KW:
        case I32_KW:
        case FLOAT:
        case IDENTIFIER:
        case NULL_KW:
            break;
        case MINUS:
            parseExpression(25);
            break;
        case LEFT_PAREN:
            parseExpression(0);
            if (lexer.currentToken.type != RIGHT_PAREN) {
                throw std::runtime_error("The ending right paren is missing");
            }
            break;
        default:
            throw std::runtime_error("Unexpected token as nud " + getTokenTypeName(token.type));
    }
}



void ExpressionParser::parseInfix(Token opToken) {
    switch (opToken.type) {
        case PLUS:
        case MINUS:
        case STAR:
        case SLASH:
        case GREATER_THAN_SIGN:
        case LESS_THAN_SIGN:
        case GREATER_EQ_THAN_SIGN:
        case LESS_EQ_THAN_SIGN:
        case DOUBLE_EQ:
        case NOT_EQ:
            parseExpression(getPrecedence(opToken.type));
            break;

        case DOT:
            if (lexer.currentToken.type != IDENTIFIER) {
                throw std::runtime_error("Should be identifier after dot");
            }
            lexer.advance();
            break;

        case LEFT_PAREN:
            if (lexer.currentToken.type != RIGHT_PAREN) {
                do {
                    parseExpression(0);

                    if (lexer.currentToken.type == COMMA) {
                        lexer.advance();
                    } else {
                        break;
                    }
                } while (true);
            }
            lexer.advance();
            break;

        default:
            throw std::runtime_error("Unknown infix operator");
    }
}




/**
 * Expression parser implementing pratt parsing algorithm
 */
void ExpressionParser::parseExpression(int precedence = 0) {
    Token tokenToProcess = lexer.currentToken;
    lexer.advance();
    nud(tokenToProcess);

    while (precedence < getPrecedence(lexer.currentToken.type)) {
        const Token opToken = lexer.currentToken;
        lexer.advance();
        parseInfix(opToken);
    }
}
