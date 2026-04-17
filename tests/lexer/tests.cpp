//
// Created by Dalibor Detko on 16/04/2026.
//

#include "tests.h"

#include <iostream>

#include "../../lexer/lexer.h"

#define LEXER_ERROR 1

void printTokenValue(Token token) {
    std::cout << "Token Value: " << token.value << std::endl;
}
FILE *file;

int test() {
    file = fopen("test.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return LEXER_ERROR;
    }

    file = fopen("test.txt", "r");

    if (file == NULL) {
        perror("Error opening file");
        return LEXER_ERROR;
    }
    while (true) {
        Token token;
        int isTokenError = getToken(&token);

        if (isTokenError) {
            printErrorCode();
            perror("lexer error");
            break;
        }
        printTokenValue(token);
        if (token.type == EOF_KW) {
            break;
        }
    }
    fclose(file);
    return 0;
}