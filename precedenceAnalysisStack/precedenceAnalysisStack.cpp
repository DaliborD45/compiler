//
// Created by Dalibor Detko on 19/04/2026.
//

#include "precedenceAnalysisStack.h"
#include <map>
#include "../helpers/helpers.h"

StackItem PrecAnalysisStack::top() {
    return stack.top();
}

StackItem PrecAnalysisStack::pop() {
    StackItem item = stack.top();
    stack.pop();
    return item;
}

void PrecAnalysisStack::push(StackItem item) {
    stack.push(item);
}
bool PrecAnalysisStack::empty() {
    return stack.empty();
}

Token PrecAnalysisStack::getTopTerminal() {
    std::stack<StackItem> temp = stack;
    while (!temp.empty()) {
        if (temp.top().type == ItemType::RAW_TOKEN) {
            return temp.top().token;
        }
        temp.pop();
    }
    throw std::runtime_error("Syntaktická chyba: Nebol nájdený žiadny terminál na zásobníku.");
}

void PrecAnalysisStack::reduceStack() {
    if (stack.empty()) return;

    const StackItem item = top();

    // PRAVIDLO: E -> i
    if (item.type == ItemType::RAW_TOKEN &&  includes({IDENTIFIER, NUMBER, F64_KW, I32_KW},item.token.type)) {
        Token t = pop().token;
        auto leaf = std::make_shared<LeafNode>(t);
        push(StackItem(leaf));
        return;
    }

    // PRAVIDLO: E -> ( E )
    if (item.type == ItemType::RAW_TOKEN && item.token.type == RIGHT_PAREN) {
        pop();

        if (stack.empty()) throw std::runtime_error("Chyba: Chýba obsah zátvoriek");
        StackItem content = pop();

        if (stack.empty() || stack.top().type != ItemType::RAW_TOKEN || stack.top().token.type != LEFT_PAREN) {
            throw std::runtime_error("Chyba: Chýba otváracia zátvorka '('");
        }
        pop();

        push(content);
        return;
    }

    // PRAVIDLO: E -> E op E
    if (item.type == ItemType::AST_NODE) {
        std::shared_ptr<ASTNode> right = pop().node;

        if (stack.empty() || stack.top().type != ItemType::RAW_TOKEN) {
            push(StackItem(right));
            return;
        }

        Token op = pop().token;

        if (stack.empty() || stack.top().type != ItemType::AST_NODE) {
            throw std::runtime_error("Chyba: Chýba ľavý operand");
        }
        std::shared_ptr<ASTNode> left = pop().node;

        auto binary = std::make_shared<BinaryNode>(op, left, right);
        push(StackItem(binary));
        return;
    }

    throw std::runtime_error("Chyba: Žiadne pravidlo pre redukciu nevyhovuje obsahu zásobníka");
}


/**
 * Created from precedence table
 */
std::map<int, std::map<int, std::string>> priorityTable = {
    { PLUS, { {PLUS, ">"}, {MINUS, ">"}, {STAR, "<"}, {SLASH,"<"}, {LESS_THAN_SIGN, ">"},{GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, { DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"}  } },
    { MINUS, { {PLUS, ">"}, {MINUS, ">"}, {STAR, "<"}, {SLASH,"<"}, {LESS_THAN_SIGN, ">"},{GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, { DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"}  } },

    { STAR, { {PLUS, ">"}, {MINUS, ">"}, {STAR, ">"}, {SLASH, ">"}, {LESS_THAN_SIGN, ">"}, {GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, {DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },
    { SLASH, { {PLUS, ">"}, {MINUS, ">"}, {STAR, ">"}, {SLASH, ">"}, {LESS_THAN_SIGN, ">"}, {GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, {DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },

    { LESS_THAN_SIGN,       { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "x"}, {GREATER_THAN_SIGN, "x"}, {LESS_EQ_THAN_SIGN, "x"}, {GREATER_EQ_THAN_SIGN, "x"}, {DOUBLE_EQ, "x"}, {NOT_EQ, "x"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },
    { GREATER_THAN_SIGN,    { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "x"}, {GREATER_THAN_SIGN, "x"}, {LESS_EQ_THAN_SIGN, "x"}, {GREATER_EQ_THAN_SIGN, "x"}, {DOUBLE_EQ, "x"}, {NOT_EQ, "x"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },
    { LESS_EQ_THAN_SIGN,    { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "x"}, {GREATER_THAN_SIGN, "x"}, {LESS_EQ_THAN_SIGN, "x"}, {GREATER_EQ_THAN_SIGN, "x"}, {DOUBLE_EQ, "x"}, {NOT_EQ, "x"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },
    { GREATER_EQ_THAN_SIGN, { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "x"}, {GREATER_THAN_SIGN, "x"}, {LESS_EQ_THAN_SIGN, "x"}, {GREATER_EQ_THAN_SIGN, "x"}, {DOUBLE_EQ, "x"}, {NOT_EQ, "x"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },

    { DOUBLE_EQ, { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "x"}, {GREATER_THAN_SIGN, "x"}, {LESS_EQ_THAN_SIGN, "x"}, {GREATER_EQ_THAN_SIGN, "x"}, {DOUBLE_EQ, "x"}, {NOT_EQ, "x"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },
    { NOT_EQ,    { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "x"}, {GREATER_THAN_SIGN, "x"}, {LESS_EQ_THAN_SIGN, "x"}, {GREATER_EQ_THAN_SIGN, "x"}, {DOUBLE_EQ, "x"}, {NOT_EQ, "x"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },

    { LEFT_PAREN, { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "<"}, {GREATER_THAN_SIGN, "<"}, {LESS_EQ_THAN_SIGN, "<"}, {GREATER_EQ_THAN_SIGN, "<"}, {DOUBLE_EQ, "<"}, {NOT_EQ, "<"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, "="}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, ">"} } },

    { RIGHT_PAREN, { {PLUS, ">"}, {MINUS, ">"}, {STAR, ">"}, {SLASH, ">"}, {LESS_THAN_SIGN, ">"}, {GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, {DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "x"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "x"}, {NUMBER, "x"}, {I32_KW, "x"}, {F64_KW, "x"}, {DOLLAR_SIGN, ">"} } },

    // Operands (IDENTIFIER, NUMBER, I32, F64) act the same
    { IDENTIFIER, { {PLUS, ">"}, {MINUS, ">"}, {STAR, ">"}, {SLASH, ">"}, {LESS_THAN_SIGN, ">"}, {GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, {DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "x"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "x"}, {NUMBER, "x"}, {I32_KW, "x"}, {F64_KW, "x"}, {DOLLAR_SIGN, ">"} } },
    { NUMBER,     { {PLUS, ">"}, {MINUS, ">"}, {STAR, ">"}, {SLASH, ">"}, {LESS_THAN_SIGN, ">"}, {GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, {DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "x"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "x"}, {NUMBER, "x"}, {I32_KW, "x"}, {F64_KW, "x"}, {DOLLAR_SIGN, ">"} } },
    { I32_KW,     { {PLUS, ">"}, {MINUS, ">"}, {STAR, ">"}, {SLASH, ">"}, {LESS_THAN_SIGN, ">"}, {GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, {DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "x"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "x"}, {NUMBER, "x"}, {I32_KW, "x"}, {F64_KW, "x"}, {DOLLAR_SIGN, ">"} } },
    { F64_KW,     { {PLUS, ">"}, {MINUS, ">"}, {STAR, ">"}, {SLASH, ">"}, {LESS_THAN_SIGN, ">"}, {GREATER_THAN_SIGN, ">"}, {LESS_EQ_THAN_SIGN, ">"}, {GREATER_EQ_THAN_SIGN, ">"}, {DOUBLE_EQ, ">"}, {NOT_EQ, ">"}, {LEFT_PAREN, "x"}, {RIGHT_PAREN, ">"}, {IDENTIFIER, "x"}, {NUMBER, "x"}, {I32_KW, "x"}, {F64_KW, "x"}, {DOLLAR_SIGN, ">"} } },

    { DOLLAR_SIGN, { {PLUS, "<"}, {MINUS, "<"}, {STAR, "<"}, {SLASH, "<"}, {LESS_THAN_SIGN, "<"}, {GREATER_THAN_SIGN, "<"}, {LESS_EQ_THAN_SIGN, "<"}, {GREATER_EQ_THAN_SIGN, "<"}, {DOUBLE_EQ, "<"}, {NOT_EQ, "<"}, {LEFT_PAREN, "<"}, {RIGHT_PAREN, "x"}, {IDENTIFIER, "<"}, {NUMBER, "<"}, {I32_KW, "<"}, {F64_KW, "<"}, {DOLLAR_SIGN, "end"} } }
};

std::string checkPrecedenceOfTokens(const Token& leftToken,const Token& rightToken) {
    if (priorityTable.count(leftToken.type)== 0 || priorityTable.count(rightToken.type)== 0) {
        printTokenObject(leftToken);
        printTokenObject(rightToken);
        throw std::runtime_error("Tokens does not exist in the precedence table");
    }

    const std::string precedence =  priorityTable[leftToken.type][rightToken.type];
    if (precedence == "x") {
        throw std::runtime_error("Wrong mix of operands " + getTokenTypeName(leftToken.type) + " and " + getTokenTypeName(rightToken.type) );
    }
    return precedence;
}

