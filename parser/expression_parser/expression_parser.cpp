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

Expression ExpressionParser::nud(Token token) {
    switch (token.type) {
        case NUMBER:
            return std::make_unique<LiteralASTNode>(NUMBER, token.value);
        case STRING:
            return std::make_unique<LiteralASTNode>(STRING, token.value);
        case F64_KW:
            return std::make_unique<LiteralASTNode>(F64_KW, token.value);
        case I32_KW:
            return std::make_unique<LiteralASTNode>(I32_KW, token.value);
        case FLOAT:
            return std::make_unique<LiteralASTNode>(FLOAT, token.value);
        case IDENTIFIER:
            return std::make_unique<LiteralASTNode>(IDENTIFIER, token.value);
        case NULL_KW:
            return std::make_unique<LiteralASTNode>(NULL_KW, token.value);
        case MINUS:
            return std::make_unique<UnaryExpressionASTNode>(MINUS, parseExpression(25));
        case LEFT_PAREN: {
            auto expr = parseExpression(0);
            if (lexer.currentToken.type != RIGHT_PAREN) {
                throw std::runtime_error("The ending right paren is missing");
            }
            return expr;
        }
        default:
            throw std::runtime_error("Unexpected token as nud " + getTokenTypeName(token.type));
    }
}



Expression ExpressionParser::parseInfix(Expression left,Token opToken) {
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
        case NOT_EQ: {
            auto right = parseExpression(getPrecedence(opToken.type));

            return std::make_unique<BinaryExpressionASTNode>(opToken.type, std::move(left), std::move(right));
        }
        case DOT: {
            if (lexer.currentToken.type != IDENTIFIER) {
                throw std::runtime_error("Should be identifier after dot");
            }
            std::string attributeName = lexer.currentToken.value;
            lexer.advance();
            return std::make_unique<MemberAccessExpressionASTNode>(std::move(left),attributeName);
        }
        case LEFT_PAREN: {
            std::vector<Expression> args;
            if (lexer.currentToken.type != RIGHT_PAREN) {
                do {
                    args.push_back(parseExpression(0));
                    if (lexer.currentToken.type == COMMA) {
                        lexer.advance();
                    } else {
                        break;
                    }
                } while (true);
            }
            lexer.advance();
            return std::make_unique<CallExpressionASTNode>(std::move(left), std::move(args));
        }
        default:
            throw std::runtime_error("Unknown infix operator");
    }
}




/**
 * Expression parser implementing pratt parsing algorithm
 */
Expression ExpressionParser::parseExpression(int precedence = 0) {
    Token tokenToProcess = lexer.currentToken;
    lexer.advance();
    auto left = nud(tokenToProcess);

    while (precedence < getPrecedence(lexer.currentToken.type)) {
        const Token opToken = lexer.currentToken;
        lexer.advance();
        left = parseInfix(std::move(left),opToken);
    }

    return left;
}
