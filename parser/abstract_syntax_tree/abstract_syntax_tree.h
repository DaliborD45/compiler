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

    //virtual destructor to avoid memory leaks
    ~ExpressionASTNode() override = default;
};
using Expression = std::unique_ptr<ExpressionASTNode>;

class LiteralASTNode : public ExpressionASTNode {
public:
    TokenTypeEnum tokenType;
    std::string value;
    LiteralASTNode(TokenTypeEnum tokenType, std::string value): ExpressionASTNode(), tokenType(tokenType), value(value) {}
};
class UnaryExpressionASTNode : public ExpressionASTNode {
    public:
    TokenTypeEnum operatorType;
    std::unique_ptr<ExpressionASTNode> operand;

    UnaryExpressionASTNode(TokenTypeEnum operatorType, std::unique_ptr<ExpressionASTNode> operand)
    : ExpressionASTNode(),operatorType(operatorType),operand(std::move(operand)) {}
};


/**
 * Node for handling aritmetic expressions
 */
class BinaryExpressionASTNode : public ExpressionASTNode {
public:
    TokenTypeEnum operatorType;
    Expression left;
    Expression right;
    BinaryExpressionASTNode(TokenTypeEnum operatorType, Expression left, Expression right)
    : ExpressionASTNode(),operatorType(operatorType),left(std::move(left)),right(std::move(right)) {}
};

class MemberAccessExpressionASTNode : public ExpressionASTNode {
public:
    Expression object;
    std::string memberName;

    MemberAccessExpressionASTNode(Expression object, std::string memberName)
    : ExpressionASTNode(),object(std::move(object)),memberName(memberName) {}
};

/**
 * Node for handling function calls inside expression tree
 */
class CallExpressionASTNode : public ExpressionASTNode {
public:
    Expression left;
    std::vector<std::unique_ptr<ExpressionASTNode>> arguments;

    CallExpressionASTNode(Expression left, std::vector<std::unique_ptr<ExpressionASTNode>> arguments)
    : ExpressionASTNode(),left(std::move(left)),arguments(std::move(arguments)) {}
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
using Statement = std::unique_ptr<StatementASTNode>;


/**
 * Return statement, altough this could be taken as an expression
 * It's much better to treat it as a statement, becuase ich function can have
 * multiple return paths
 */
class ReturnStatementASTNode : public StatementASTNode {
public:
    Expression returnExpression;

    ReturnStatementASTNode() = default;
    // ReturnStatementASTNode(Expression returnExpression):
    // StatementASTNode(),returnExpression(std::move(returnExpression)) {}
};

/**
 * Node for declaration
 */
class DeclarationStatementASTNode : public StatementASTNode {
public:
    std::string variableName;
    bool isConstant;
    TokenTypeEnum variableType;
    std::unique_ptr<ExpressionASTNode> expression;

    DeclarationStatementASTNode() = default;
    // DeclarationStatementASTNode(std::string name, bool isConst, std::unique_ptr<ExpressionASTNode> init, TokenTypeEnum type)
    //       : StatementASTNode(),
    //         variableName(std::move(name)),
    //         isConstant(isConst),
    //         expression(std::move(init)),
    //         variableType(type)
    // {}
};

/**
 * Node for assignment
 */
class AssignStatementASTNode : public StatementASTNode {
public:
    std::string variableName;
    std::unique_ptr<ExpressionASTNode> expression;

    AssignStatementASTNode() = default;
    // AssignStatementASTNode(std::string variableName, std::unique_ptr<ExpressionASTNode> expression)
    //     : StatementASTNode(),variableName(std::move(variableName)), expression(std::move(expression)) {}
};

/**
 * This type of statement can occur when there is just function
 * call on the line
 */
class FunctionCallStatementASTNode : public StatementASTNode {
public:
    Expression functionExpression;
    FunctionCallStatementASTNode() = default;
    // FunctionCallStatementASTNode(Expression functionExpression):
    // StatementASTNode(),functionExpression(std::move(functionExpression)) {}
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
    std::string pipeIdentifier;

    IfStatementASTNode() = default;
    // IfStatementASTNode(std::unique_ptr<ExpressionASTNode> condition,std::vector<std::unique_ptr<StatementASTNode>> thenStatements,std::vector<std::unique_ptr<StatementASTNode>> elseStatements)
    //     : StatementASTNode(),condition(std::move(condition)), thenStatements(std::move(thenStatements)), elseStatements(std::move(elseStatements)) {}

};

/**
 * Node for handling while statements
 */
class WhileStatementASTNode : public StatementASTNode {
public:
    std::unique_ptr<ExpressionASTNode> condition;
    StatementsList whileStatements;
    std::string pipeIdentifier;

    WhileStatementASTNode() = default;
    // WhileStatementASTNode(std::unique_ptr<ExpressionASTNode> condition,std::vector<std::unique_ptr<StatementASTNode>> whileStatements)
    // : StatementASTNode(),condition(std::move(condition)), whileStatements(std::move(whileStatements)) {}
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

    FunctionASTNode(
        std::string functionName,
        ValueType returnType,
        ParametersList parameters,
        StatementsList statements
    ) : ASTNode(),
        functionName(std::move(functionName)),
        returnType(returnType),
        parameters(std::move(parameters)),
        statements(std::move(statements))
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
