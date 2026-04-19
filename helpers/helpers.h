//
// Created by Dalibor Detko on 17/04/2026.
//

#ifndef COMPILER_HELPERS_H
#define COMPILER_HELPERS_H

#include <string>
#include "../lexer/lexer.h"


std::string getTokenTypeName(int index);
void printTokenObject(const Token& token);

template<typename T>
bool includes(const std::vector<T>& vec, const T& value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}
inline std::vector baseTypesVector = {F64_KW, U8_KW, I32_KW, VOID_KW};


#endif //COMPILER_HELPERS_H
