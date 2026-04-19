//
// Created by Dalibor Detko on 17/04/2026.
//

#include "parser.h"
#include <ranges>
#include <iostream>
#include "../helpers/helpers.h"


int Parser::processToken(Token &token) {
    if (token.type == TokenTypeEnum::COLON) {
        std::cerr << token.value << std::endl;
    }
    return 0;
}

//TODO: make expression parsing with precedence analysis
void Parser::processExpression() {

}

void Parser::processImport() {
    //for now, we demand the import of the core library for every program
    expectToken(CONST_KW);
    expectToken(IDENTIFIER);
    expectToken(EQ);
    expectToken(AT_IMPORT);
    expectToken(LEFT_PAREN);
    expectToken(STRING);
    expectToken(RIGHT_PAREN);
    expectToken(SEMICOLON);
}

void Parser::processEndOfFile() {
   expectToken({EOF_KW});
}

//
// void Parser::processParameter() {
//     expectToken(IDENTIFIER);
// }
//
// void Parser::processParameterList() {
//     processParameter();
// }


bool Parser::isMatching(const std::vector<TokenTypeEnum>& typeArray, bool shouldReturnTokenBack = true){
    Token token;
    lexer.getToken(token);
    bool is_matching = false;
    if (includes(typeArray, token.type)) {
        is_matching = true;
    }
    if (shouldReturnTokenBack) {
        lexer.returnToken(token);
    }
    return is_matching;
}

void Parser::processReturnType() {
    Token token;
    //parse optional type
    if (isMatching({TokenTypeEnum::QUESTION_MARK})) {
        lexer.getToken(token);
    }
    //parse base type
    if (isMatching(baseTypesVector)) {
        lexer.getToken(token);
    }
}


void Parser::processDeclarationStatement() {
    expectToken(IDENTIFIER);
    //Process declaration tail
    if (isMatching({COLON})) {
        Token token;
        lexer.getToken(token);

        //TODO: this should be refactored to separate function process type
        if (isMatching(baseTypesVector)) {
            lexer.getToken(token);
        }
        else {
            throw std::runtime_error("Lexer::processDeclarationStatement()");
        }
    }

    //process after tail
    expectToken(EQ);
    processExpression();
    expectToken(SEMICOLON);
}

void Parser::processAssignOrCallStatement() {
    // AssignOrCallTail
    if (isMatching({EQ})) {
        Token token;
        lexer.getToken(token);
        processExpression();
        expectToken(SEMICOLON);
    }
    else {
        expectToken(LEFT_PAREN);
        //TODO: implement argument list
        // processArgList();
        expectToken(RIGHT_PAREN);
        expectToken(SEMICOLON);
    }
}

void Parser::processIfStatement() {
    expectToken(LEFT_PAREN);
    processExpression();
    expectToken(RIGHT_PAREN);
    processBlock();

    //process else statement
    if (isMatching({ELSE_KW})) {
        Token token;
        lexer.getToken(token);
        processBlock();
    }
    //else epsilon
}

void Parser::processWhileStatement() {
    expectToken(LEFT_PAREN);
    processExpression();
    expectToken(RIGHT_PAREN);
    processBlock();
    if (isMatching({ELSE_KW})) {
        Token token;
        lexer.getToken(token);
        processBlock();
    }
    //else epsilon
}

void Parser::processReturnStatement() {
    //returnTail
    if (isMatching({SEMICOLON})) {
        Token token;
        lexer.getToken(token);
        return;
    }else {
        processExpression();
        expectToken(SEMICOLON);
    }
}


void Parser::processStatementList() {
    Token token;
    lexer.getToken(token);
    if (includes({TokenTypeEnum::CONST_KW, TokenTypeEnum::VAR_KW}, token.type)) {
        // lexer.returnToken(token);
        processDeclarationStatement();
        processStatementList();
    }
    else if (token.type == TokenTypeEnum::IDENTIFIER) {
        // lexer.returnToken(token);
        processAssignOrCallStatement();
        processStatementList();
    }
    else if (token.type == TokenTypeEnum::IF_KW) {
        // lexer.returnToken(token);
        processIfStatement();
        processStatementList();
    }
    else if (token.type == TokenTypeEnum::WHILE_KW) {
        processWhileStatement();
        processStatementList();
    }
    else if (token.type == TokenTypeEnum::RETURN_KW) {
        processReturnStatement();
        processStatementList();
    }
    else if (token.type == TokenTypeEnum::LEFT_CURLY_PAREN) {
        lexer.returnToken(token);
        processBlock();
        processStatementList();
    }
    else {
        lexer.returnToken(token);
    }
}

void Parser::processBlock() {
    expectToken(TokenTypeEnum::LEFT_CURLY_PAREN);
    processStatementList();
    expectToken(TokenTypeEnum::RIGHT_CURLY_PAREN);
}

void Parser::processFunctionList() {
    Token token;
    if (!isMatching({PUB_KW})) {
        return;
    }
    expectToken(TokenTypeEnum::PUB_KW);
    expectToken(TokenTypeEnum::FN_KW);
    expectToken(TokenTypeEnum::IDENTIFIER);
    expectToken(TokenTypeEnum::LEFT_PAREN);
    //TODO: add implementation for parameters
    // processParameterList();
    expectToken(TokenTypeEnum::RIGHT_PAREN);
    processReturnType();
    processBlock();

    // recursively call again
    processFunctionList();
}

void Parser::expectToken(TokenTypeEnum type) {
    Token token;
    lexer.getToken(token);
    if (token.type == type) {
        return;
    }else {
        printTokenObject(token);
        throw std::runtime_error("The expected token " + getTokenTypeName(type) + " was not found");
    }
}


void Parser::createTree() {
    Token token;
    processImport();
    processFunctionList();
    processEndOfFile();
}


