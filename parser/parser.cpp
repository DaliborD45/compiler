//
// Created by Dalibor Detko on 17/04/2026.
//

#include "parser.h"
#include "../helpers/helpers.h"
#include "../lexer/lexer.h"
#include "abstract_syntax_tree/abstract_syntax_tree.h"


void Parser::processExpression() {
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
    return;
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


void Parser::processDeclarationStatement() {
    lexer.expectToken({IDENTIFIER});
    //Process declaration tail
    if (lexer.isMatching({COLON})) {
        lexer.advance();
        lexer.expectToken(baseTypesVector);
    }
    //process after tail
    lexer.expectToken({EQ});
    lexer.advance();
    processExpression();

}


void Parser::processAssignOrCallStatement() {
    // AssignOrCallTail
    if (lexer.isMatching({EQ})) {
        // We need to skip equal, so first advance will push you to EQ, and second will skip it to the expression
        lexer.advance();
        lexer.advance();
    }
    processExpression();

}

void Parser::processIfStatement() {
    //processExpression is also handling brackets
    lexer.advance();
    processExpression();
    processPipeAfterCondition();
    processBlock();

    //process else statement
    if (lexer.isMatching({ELSE_KW})) {
        lexer.advance();
        processBlock();
    }
    //else epsilon
}

void Parser::processPipeAfterCondition() {
    if (lexer.isMatching({PIPE_SIGN})) {
        //for pipe
        lexer.advance();
        if (!lexer.isMatching({IDENTIFIER})) {
            throw std::runtime_error("Identifier after pipe expected");
        }
        //for id inside pipe
        lexer.advance();
        if (!lexer.isMatching({PIPE_SIGN})) {
            throw std::logic_error("Expected 'PIPE_SIGN' after 'PIPE_SIGN'");
        }
        //for ending pipe sign
        lexer.advance();
    }
}


void Parser::processWhileStatement() {
    //processExpression is also handling brackets
    lexer.advance();
    //now process just expression
    processExpression();
    processPipeAfterCondition();
    processBlock();
    if (lexer.isMatching({ELSE_KW})) {
        lexer.advance();
        processBlock();
    }
    //else epsilon
}

void Parser::processReturnStatement() {
    //returnTail
    if (lexer.isMatching({SEMICOLON})) {
        lexer.advance();
        return;
    }else {
        lexer.advance();
        processExpression();
    }
}


void Parser::processStatementList() {
    lexer.advance();
    if (includes({TokenTypeEnum::CONST_KW, TokenTypeEnum::VAR_KW}, lexer.currentToken.type)) {
        processDeclarationStatement();
        processStatementList();
    }
    else if (lexer.currentToken.type == TokenTypeEnum::IDENTIFIER) {
        processAssignOrCallStatement();
        processStatementList();
    }
    else if (lexer.currentToken.type == TokenTypeEnum::IF_KW) {
        processIfStatement();
        processStatementList();
    }
    else if (lexer.currentToken.type == TokenTypeEnum::WHILE_KW) {
        processWhileStatement();
        processStatementList();
    }
    else if (lexer.currentToken.type == TokenTypeEnum::RETURN_KW) {
        processReturnStatement();
        processStatementList();
    }
    else if (lexer.currentToken.type == TokenTypeEnum::LEFT_CURLY_PAREN) {
        lexer.returnToken(lexer.currentToken);
        processBlock();
        processStatementList();
    }
    else {
        lexer.returnToken(lexer.currentToken);
    }
}

void Parser::processBlock() {
    lexer.expectToken({LEFT_CURLY_PAREN});
    processStatementList();
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
    processBlock();

    // programASTNode->functions.push_back(std::make_unique<FunctionASTNode>(functionName,returnType, parametersList,statementsList ));
    // recursively call again
    processFunctionList();

    return;
}


void Parser::createTree() {
    processImport();
    processFunctionList();
    processEndOfFile();
}




