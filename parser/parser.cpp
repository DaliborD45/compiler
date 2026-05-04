//
// Created by Dalibor Detko on 17/04/2026.
//

#include "parser.h"
#include "../helpers/helpers.h"
#include "../lexer/lexer.h"
#include "abstract_syntax_tree/abstract_syntax_tree.h"


void Parser::processExpression(Expression& expression) {
    expressionParser.parseExpression(0);
}

void Parser::processImport() {
    //for now, we demand the import of the core library for every program
    lexer.expectToken({CONST_KW});
    lexer.expectToken({IDENTIFIER});
    std::string identifierName = lexer.currentToken.value;
    lexer.expectToken({EQ});
    lexer.expectToken({AT_IMPORT});
    lexer.expectToken({LEFT_PAREN});
    lexer.expectToken({STRING});
    std::string importPath = lexer.currentToken.value;
    lexer.expectToken({RIGHT_PAREN});
    lexer.expectToken({SEMICOLON});

    //Create node for AST
    programASTNode->imports.push_back(std::make_unique<ImportASTNode>(identifierName, importPath));
}

void Parser::processEndOfFile() {
   lexer.expectToken({EOF_KW});
}

ValueType Parser::processReturnType() {
    ValueType returnType = {.isOptional = false,};
    //parse optional type
    if (lexer.isMatching({QUESTION_MARK})) {
        lexer.advance();
        returnType.isOptional = true;
    }

    lexer.expectToken(baseTypesVector);
    returnType.type = lexer.currentToken.type;

    return returnType;
}


void Parser::processDeclarationStatement(std::unique_ptr<DeclarationStatementASTNode>& declarationStatement) {
    lexer.expectToken({IDENTIFIER});
    declarationStatement->variableName = lexer.currentToken.value;

    //Process declaration tail
    if (lexer.isMatching({COLON})) {
        lexer.advance();
        lexer.expectToken(baseTypesVector);
        declarationStatement->variableType = lexer.currentToken.type;
    }else {
        //TODO:implicit typing of the language,
        declarationStatement->variableType = IDENTIFIER;
    }

    //process after tail
    lexer.expectToken({EQ});
    lexer.advance();
    processExpression(declarationStatement->expression);
}


void Parser::processIfStatement(std::unique_ptr<IfStatementASTNode>& ifStatement) {
    //processExpression is also handling brackets
    lexer.advance();
    processExpression(ifStatement->condition);
    processPipeAfterCondition(ifStatement->pipeIdentifier);
    processBlock(ifStatement->thenStatements);

    //process else statement
    if (lexer.isMatching({ELSE_KW})) {
        lexer.advance();
        processBlock(ifStatement->elseStatements);
    }
    //else epsilon
}

void Parser::processPipeAfterCondition(std::string& pipeIdentifier) {
    if (lexer.isMatching({PIPE_SIGN})) {
        //for pipe
        lexer.advance();
        if (!lexer.isMatching({IDENTIFIER})) {
            throw std::runtime_error("Identifier after pipe expected");
        }
        //for id inside pipe
        lexer.advance();
        pipeIdentifier = lexer.currentToken.value;
        if (!lexer.isMatching({PIPE_SIGN})) {
            throw std::logic_error("Expected 'PIPE_SIGN' after 'PIPE_SIGN'");
        }
        //for ending pipe sign
        lexer.advance();
    }else {
        //if there is no pipe, just return empty string
        pipeIdentifier = "";
    }
}


void Parser::processWhileStatement(std::unique_ptr<WhileStatementASTNode>& whileStatement) {
    //processExpression is also handling brackets
    lexer.advance();
    //now process just expression
    processExpression(whileStatement->condition);
    processPipeAfterCondition(whileStatement->pipeIdentifier);
    //process body of the while statement
    processBlock(whileStatement->whileStatements);
}

void Parser::processReturnStatement(std::unique_ptr<ReturnStatementASTNode>& returnStatement) {
    //returnTail
    if (lexer.isMatching({SEMICOLON})) {
        lexer.advance();
        //if the return statement is just return; we assign null pointer
        returnStatement->returnExpression = nullptr;
        return;
    }else {
        lexer.advance();
        processExpression(returnStatement->returnExpression);
    }
}


void Parser::processStatementList(StatementsList& statementsList) {
    lexer.advance();
    if (includes({TokenTypeEnum::CONST_KW, TokenTypeEnum::VAR_KW}, lexer.currentToken.type)) {
        auto declarationStatement = std::make_unique<DeclarationStatementASTNode>();
        declarationStatement->isConstant = lexer.currentToken.type == TokenTypeEnum::CONST_KW;
        //process the statement
        processDeclarationStatement(declarationStatement);
        //push into the list
        statementsList.push_back(std::move(declarationStatement));
        processStatementList(statementsList);
    }
    else if (lexer.currentToken.type == TokenTypeEnum::IDENTIFIER) {
        if (lexer.isMatching({EQ})) {
            auto assignStatement = std::make_unique<AssignStatementASTNode>();
            //we are still on identifier so we will assign name
            assignStatement->variableName = lexer.currentToken.value;
            // We need to skip equal, so first advance will push you to EQ, and second will skip it to the expression
            lexer.advance();
            lexer.advance();
            processExpression(assignStatement->expression);
            statementsList.push_back(std::move(assignStatement));
        }
        else {
            auto callStatement = std::make_unique<FunctionCallStatementASTNode>();
            processExpression(callStatement->functionExpression);
            statementsList.push_back(std::move(callStatement));
        }
        //recursively call again
        processStatementList(statementsList);
    }
    else if (lexer.currentToken.type == TokenTypeEnum::IF_KW) {
        auto ifStatement = std::make_unique<IfStatementASTNode>();
        processIfStatement(ifStatement);
        //push into the list
        statementsList.push_back(std::move(ifStatement));
        processStatementList(statementsList);
    }
    else if (lexer.currentToken.type == TokenTypeEnum::WHILE_KW) {
        auto whileStatement = std::make_unique<WhileStatementASTNode>();
        //process statement
        processWhileStatement(whileStatement);
        //push into the list
        statementsList.push_back(std::move(whileStatement));
        //call again recursively
        processStatementList(statementsList);
    }
    else if (lexer.currentToken.type == TokenTypeEnum::RETURN_KW) {
        auto returnStatement = std::make_unique<ReturnStatementASTNode>();
        processReturnStatement(returnStatement);
        //push into the list
        statementsList.push_back(std::move(returnStatement));
        //call again recursively
        processStatementList(statementsList);
    }
    else {
        lexer.returnToken(lexer.currentToken);
    }
}

void Parser::processBlock(StatementsList& statementsList) {
    lexer.expectToken({LEFT_CURLY_PAREN});
    processStatementList(statementsList);
    lexer.expectToken({RIGHT_CURLY_PAREN});
}



void Parser::processParameter(ParametersList& parametersList) {
    lexer.expectToken({IDENTIFIER});
    std::string identifier = lexer.currentToken.value;
    lexer.expectToken({COLON});
    lexer.expectToken(baseTypesVector);
    ValueType paramType =  {.type = lexer.currentToken.type,.isOptional = false };

    parametersList.push_back(std::make_unique<ParameterNode>(identifier, paramType));
    // parametersTail
    if (lexer.isMatching({COMMA})) {
        lexer.advance();
        processParameter(parametersList);
    }
}

void Parser::processParameterList(ParametersList& parametersList) {
    if (lexer.isMatching({IDENTIFIER})) {
        processParameter(parametersList);
    }
}

void Parser::processFunctionList() {
    if (!lexer.isMatching({PUB_KW})) {
        return;
    }
    ParametersList parametersList;
    StatementsList statementsList;

    lexer.expectToken({PUB_KW});
    lexer.expectToken({FN_KW});
    lexer.expectToken({IDENTIFIER});
    std::string functionName = lexer.currentToken.value;
    lexer.expectToken({LEFT_PAREN});
    processParameterList(parametersList);
    lexer.expectToken({RIGHT_PAREN});
    ValueType returnType = processReturnType();
    processBlock(statementsList);
    //Create function node in the main program node
    programASTNode->functions.push_back(std::make_unique<FunctionASTNode>(
        functionName,
        returnType,
        std::move(parametersList),
        std::move(statementsList)
    ));

    // recursively call again
    processFunctionList();
}


void Parser::createTree() {
    processImport();
    processFunctionList();
    processEndOfFile();
}




