//
// Created by Dalibor Detko on 17/04/2026.
//

#include "helpers.h"
#include <iostream>
#include <string>
#include <map>
#include "../lexer/lexer.h"

std::string getTokenTypeName(const int index)
{
    static const char* tokenTypeNames[] = {
        "CONST_KW",
        "IF_KW",
        "ELSE_KW",
        "FN_KW",
        "I32_KW",
        "F64_KW",
        "NULL_KW",
        "PUB_KW",
        "RETURN_KW",
        "U8_KW",
        "VAR_KW",
        "VOID_KW",
        "WHILE_KW",
        "PLUS",
        "MINUS",
        "STAR",
        "SLASH",
        "PERCENT",
        "EQ",
        "DOUBLE_EQ",
        "EXCLAMATION",
        "DOT",
        "STRING",
        "IDENTIFIER",
        "NUMBER",
        "FLOAT",
        "EOF_KW",
        "AT",
        "SEMICOLON",
        "LEFT_PAREN",
        "RIGHT_PAREN",
        "LEFT_CURLY_PAREN",
        "RIGHT_CURLY_PAREN",
        "LEFT_SQUARE_PAREN",
        "RIGHT_SQUARE_PAREN",
        "NEW_LINE",
        "TAB",
        "CARRIAGE_RETURN",
        "COMMA",
        "COLON",
        "AT_IMPORT",
        "QUESTION_MARK",
        "GREATER_THAN_SIGN",
        "LESS_THAN_SIGN",
        "GREATER_EQ_THAN_SIGN",
        "LESS_EQ_THAN_SIGN",
        "PIPE_SIGN",
        "HASHTAG",
        "NOT_EQ",
        "DOLLAR_SIGN"
    };

    int numTokenTypes = sizeof(tokenTypeNames) / sizeof(tokenTypeNames[0]);
    if (index >= 0 && index < numTokenTypes)
    {
        return tokenTypeNames[index];
    }

    return "Invalid index" + std::to_string(index);
}


void printTokenObject(const Token& token) {
    std::cout << "{type:" + getTokenTypeName(token.type) + " value:" + token.value + "}" << std::endl;
}




