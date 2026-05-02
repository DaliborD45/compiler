//
// Created by Dalibor Detko on 29/04/2026.
//

#include "expression_parser.h"
#include "../../parser/parser.h"
#include "vector"
#include "../../helpers/helpers.h"
#include <utility> // std::pair


/**
 * INFIX: operator is between operands (*, /, ., (,)
 * PREFIX: operator is in front of operand (-5, +4)
 */


int getPrecedence(TokenTypeEnum type) {
    switch (type) {
        case DOT:    return 40; // Bodka má najvyššiu prioritu
        case LEFT_PAREN: return 40; // Volanie funkcie má rovnakú prioritu ako bodka
        case STAR:
        case SLASH:  return 20; // Násobenie/Delenie
        case PLUS:
        case MINUS:  return 10; // Sčítanie/Odčítanie
        default:     return 0;  // Pre tokeny, ktoré nie sú infixové operátory
    }
}
std::vector<TokenTypeEnum> operators = {PLUS, MINUS, STAR, SLASH};


void ExpressionParser::nud(Token token) {
    switch (token.type) {
        case NUMBER:
        case IDENTIFIER:
            // Sú to literály/premenné, nepotrebujú nič ďalšie. Iba sme ich zjedli.
            break;
        case MINUS:
            // Unárne mínus (napr. -5). Spracujeme pravú stranu s vyššou prioritou.
            parseExpression(25);
            break;
        case LEFT_PAREN:
            // Zátvorkový výraz: ( 1 + 2 )
            parseExpression(0); // Vnútri môže byť výraz s akoukoľvek prioritou
            lexer.expectToken({RIGHT_PAREN});
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
            // Štandardná binárna operácia. Zjeme pravú stranu s prioritou tohto operátora.
            parseExpression(getPrecedence(opToken.type));
            break;

        case DOT:
            // Pristup k vlastnosti: napr. "lavy_vyraz . vlastnost"
            // Po bodke MUSÍ nasledovať presne jeden identifikátor.
            if (currentToken.type != IDENTIFIER) {
                throw std::runtime_error("Should be identifier after dot");
            }
            lexer.getToken(currentToken);
            break;

        case LEFT_PAREN:
            // Volanie funkcie: napr. "lavy_vyraz ( arg1, arg2 )"
            // Sme dnu v zátvorkách, parsujeme argumenty oddelené čiarkou
            if (currentToken.type != RIGHT_PAREN) {
                do {
                    parseExpression(0); // Argument môže byť akýkoľvek výraz

                    if (lexer.isMatching({COMMA})) {
                        lexer.getToken(currentToken);
                    } else {
                        break; // Ak nie je čiarka, musí nasledovať pravá zátvorka
                    }
                } while (true);
            }
            //here it is as a right parent
            lexer.getToken(currentToken);
            break;

        default:
            throw std::runtime_error("Neznamy infix operator!");
    }
}




/**
 * Expression parser implementing pratt parsing algorithm
 */
void ExpressionParser::parseExpression(int precedence = 0) {
    // 1. Zoberieme prvý token a spracujeme ho ako PREFIX (NUD)
    Token tokenToProcess = currentToken;
    lexer.getToken(currentToken); // Posunieme sa na ďalší (lookahead)
    nud(tokenToProcess);

    // 2. Kým má aktuálny token VÄČŠIU silu ako naša aktuálna priorita...
    while (precedence < getPrecedence(currentToken.type)) {
        const Token opToken = currentToken;
        lexer.getToken(currentToken);
        parseInfix(opToken);
    }

}
