//
// Created by Dalibor Detko on 03/05/2026.
//

#ifndef COMPILER_ABSTRACT_SYNTAX_TREE_H
#define COMPILER_ABSTRACT_SYNTAX_TREE_H
#include <memory>
#include <vector>

#include "../../lexer/lexer.h"




/**
 * Default class from which every will inherit
 * The accepts method serves for Visitor design
 * pattern
 */
class ASTNode {
public:
    ASTNode() = default;
    virtual ~ASTNode() = default;
    //here will be later method accept(class Visitor)
};

/**
 * struct defining type
 */
struct ValueType {
    TokenTypeEnum type;
    bool isOptional;
};

/**
 * Node that handles expressions in AST
 */
class ExpressionASTNode : public ASTNode {
public:
    using ASTNode::ASTNode;
};

/**
 * Leaf node for building expression tree
 */
class IntegerLiteralASTNode : public ExpressionASTNode {
    public:
        int value;
    IntegerLiteralASTNode(int value)
    : ExpressionASTNode(),value(value) {}
};

/**
 * Leaf node for string
 */
class StringLiteralASTNode : public ExpressionASTNode {
  public:
    std::string value;
    StringLiteralASTNode(std::string value): ExpressionASTNode(),value(std::move(value)) {}
};

/**
 * Leaf node for handling identifiers
 */
class IdentifierASTNode : public ExpressionASTNode {
public:
    std::string name;
    IdentifierASTNode(std::string name): ExpressionASTNode(),name(std::move(name)) {}
};

/**
 * Node for handling aritmetic expressions
 */
class OperatorASTNode : public ExpressionASTNode {
public:
    TokenTypeEnum operatorType;
    std::unique_ptr<ExpressionASTNode> left;
    std::unique_ptr<ExpressionASTNode> right;
    OperatorASTNode(TokenTypeEnum operatorType, std::unique_ptr<ExpressionASTNode> left, std::unique_ptr<ExpressionASTNode> right)
    : ExpressionASTNode(),operatorType(operatorType),left(std::move(left)),right(std::move(right)) {}
};

/**
 * Node for handling function calls inside expression tree
 */
class CallExpressionASTNode : public ExpressionASTNode {
public:
    std::string functionName;
    std::vector<std::unique_ptr<ExpressionASTNode>> arguments;

    CallExpressionASTNode(std::string functionName, std::vector<std::unique_ptr<ExpressionASTNode>> arguments)
    : ExpressionASTNode(),functionName(std::move(functionName)),arguments(std::move(arguments)) {}
};

/**
 * Node of AST that handles statements such as
 * If or While
 */
class StatementASTNode : public ASTNode {
public:
    using ASTNode::ASTNode;
};
//Type
using StatementsList = std::vector<std::unique_ptr<StatementASTNode>>;


/**
 * Node for declaration
 */
class DeclarationASTNode : public StatementASTNode {
public:
    std::string variableName;
    bool isConstant;
    TokenTypeEnum variableType;
    std::unique_ptr<ExpressionASTNode> expression;

    DeclarationASTNode(std::string name, bool isConst, std::unique_ptr<ExpressionASTNode> init, TokenTypeEnum type)
          : StatementASTNode(),
            variableName(std::move(name)),
            isConstant(isConst),
            expression(std::move(init)),
            variableType(type)
    {}
};

/**
 * Node for assignment
 */
class AssignStatementASTNode : public StatementASTNode {
public:
    std::string variableName;
    std::unique_ptr<ExpressionASTNode> expression;

    AssignStatementASTNode(std::string variableName, std::unique_ptr<ExpressionASTNode> expression)
        : StatementASTNode(),variableName(std::move(variableName)), expression(std::move(expression)) {}
};


/**
 * Node for handling if statements
 * supporting else block
 */
class IfStatementASTNode : public StatementASTNode {
public:
    std::unique_ptr<ExpressionASTNode> condition;
    StatementsList thenStatements;
    StatementsList elseStatements;

    IfStatementASTNode(std::unique_ptr<ExpressionASTNode> condition,std::vector<std::unique_ptr<StatementASTNode>> thenStatements,std::vector<std::unique_ptr<StatementASTNode>> elseStatements)
        : StatementASTNode(),condition(std::move(condition)), thenStatements(std::move(thenStatements)), elseStatements(std::move(elseStatements)) {}
};

/**
 * Node for handling while statements
 */
class WhileStatementASTNode : public StatementASTNode {
public:
    std::unique_ptr<ExpressionASTNode> condition;
    StatementsList whileStatements;

    WhileStatementASTNode(std::unique_ptr<ExpressionASTNode> condition,std::vector<std::unique_ptr<StatementASTNode>> whileStatements)
    : StatementASTNode(),condition(std::move(condition)), whileStatements(std::move(whileStatements)) {}
};

/**
 * Node for handling import statement
 * for now, we only support default ifj import, but
 * this can be extended to any import
 */
class ImportASTNode : public ASTNode {
public:
    using ASTNode::ASTNode;
    std::string variableName;
    std::string importPath;

    ImportASTNode(std::string variableName, std::string importPath)
        :variableName(std::move(variableName)), importPath(std::move(importPath)) {};
};

class ParameterNode : public ASTNode {
public:
    using ASTNode::ASTNode;
    std::string parameterName;
    ValueType parameterType;

    ParameterNode(std::string parameterName, ValueType parameterType)
    : ASTNode(),
    parameterName(std::move(parameterName)),
    parameterType(parameterType) {}
};


//Types
using ParametersList = std::vector<std::unique_ptr<ParameterNode>>;


/**
 * Node for handling function such as pub something()
 */
class FunctionASTNode : public ASTNode {
public:
    using ASTNode::ASTNode;

    std::string functionName;
    ValueType returnType;
    ParametersList parameters;
    StatementsList statements;
    std::unique_ptr<ExpressionASTNode> returnValue;

    FunctionASTNode(
        std::string functionName,
        ValueType returnType,
        ParametersList parameters,
        StatementsList statements,
        std::unique_ptr<ExpressionASTNode> returnValue
    ) : ASTNode(),
        functionName(std::move(functionName)),
        returnType(returnType),
        parameters(std::move(parameters)),
        statements(std::move(statements)),
        returnValue(std::move(returnValue))
    {}
};

/**
 * Main node of the program, serving as root of AST
 */
class ProgramASTNode : public ASTNode {
public:
    using ASTNode::ASTNode;
    std::vector<std::unique_ptr<ImportASTNode>> imports;
    std::vector<std::unique_ptr<FunctionASTNode>> functions;

};




#endif //COMPILER_ABSTRACT_SYNTAX_TREE_H
