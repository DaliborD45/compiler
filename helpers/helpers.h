//
// Created by Dalibor Detko on 17/04/2026.
//

#ifndef COMPILER_HELPERS_H
#define COMPILER_HELPERS_H

#include <string>
#include "../lexer/lexer.h"
#include <iostream>

std::string getTokenTypeName(int index);
void printTokenObject(const Token& token);

template<typename T>
bool includes(const std::vector<T>& vec, const T& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}
inline std::vector baseTypesVector = {F64_KW, U8_KW, I32_KW, VOID_KW};

template <typename T>
void printVector(const std::vector<T>& v) {
    std::cout << "{ ";
    for (size_t i = 0; i < v.size(); i++) {
        std::cout << getTokenTypeName(v[i]) << " ";
        if (i + 1 < v.size()) std::cout << ", ";
    }
    std::cout << " }" << std::endl;
}


/**
 * Takes next token from Lexer and compares match with the tokens in @param typeArray
 * if not matching, it raises error
 */
void expectToken(const std::vector<TokenTypeEnum>& typeArray);
/**
 * Checks whether the next token from Lexer is matching one of elements expected in @param typeArray,
 * @param shouldReturnTokenBack if set to false, the token will not be returned
 * back to the file
*/
bool isMatching(const std::vector<TokenTypeEnum>& typeArray, bool shouldReturnTokenBack);

#endif //COMPILER_HELPERS_H
