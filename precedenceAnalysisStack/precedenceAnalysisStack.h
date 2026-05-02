//
// Created by Dalibor Detko on 19/04/2026.
//

#ifndef COMPILER_PRECEDENCEANALYSISSTACK_H
#define COMPILER_PRECEDENCEANALYSISSTACK_H
#include "../lexer/lexer.h"




struct ASTNode {
    virtual ~ASTNode() = default;
};

struct LeafNode : public ASTNode {
    Token token;

    LeafNode(Token t) : token(t) {}
};

struct BinaryNode : public ASTNode {
    Token op;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;

    BinaryNode(Token oper, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r)
        : op(oper), left(std::move(l)), right(std::move(r)) {}
};


enum class ItemType {
    RAW_TOKEN,
    AST_NODE
};

struct StackItem {
    ItemType type;

    Token token;
    std::shared_ptr<ASTNode> node;

    StackItem(Token t) {
        type = ItemType::RAW_TOKEN;
        token = t;
        node = nullptr;
    }

    StackItem(std::shared_ptr<ASTNode> n) {
        type = ItemType::AST_NODE;
        node = n;
    }
};

class PrecAnalysisStack {
private:
    std::stack<StackItem> stack;

public:
    /**
     * @return The most top Non-Terminal symbol
     */
    StackItem top();

    /**
     * @return Pops the most top Non-Terminal symbol
     */
    StackItem pop();

    /**
     * Pushes item into the stack
     */
    void push(StackItem item);

    /**
     * Returns very top terminal symbol
     */
    Token getTopTerminal();

    bool empty();

    /**
     * Performs reduction based on types of tokens
     * stack has
     */
    void reduceStack();
};

std::string checkPrecedenceOfTokens(const Token& leftToken,const Token& rightToken);

inline std::vector<TokenTypeEnum> allowedTokenTypes = {PLUS, MINUS, STAR, SLASH, LESS_THAN_SIGN, GREATER_THAN_SIGN,
    LESS_EQ_THAN_SIGN,GREATER_EQ_THAN_SIGN,DOUBLE_EQ,NOT_EQ,LEFT_PAREN,RIGHT_PAREN,IDENTIFIER,NUMBER,I32_KW, F64_KW,DOLLAR_SIGN };

#endif
