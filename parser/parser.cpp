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

void Parser::processExpression(){}

//TOOD: fix tuto, ze ono to musi vratit aj ten token naspat
void Parser::processImport() {
    Token token;
    lexer.getToken(token);
    if (token.type != TokenTypeEnum::CONST_KW) {
        return;
    }

    lexer.getToken(token);
    if (token.type == TokenTypeEnum::IDENTIFIER) {
        return;
    }

    lexer.getToken(token);
    if (token.type == TokenTypeEnum::EQ) {
        return;
    }

    lexer.getToken(token);
    if (token.type == TokenTypeEnum::AT_IMPORT) {
        return;
    }

    lexer.getToken(token);
    if (token.type == TokenTypeEnum::LEFT_PAREN) {
        return;
    }

    lexer.getToken(token);
    if (token.type == TokenTypeEnum::STRING) {
        return;
    }

    lexer.getToken(token);
    if (token.type == TokenTypeEnum::RIGHT_PAREN) {
        return;
    }

    lexer.getToken(token);
    if (token.type == TokenTypeEnum::SEMICOLON) {
        return;
    }
}

void Parser::processEndOfFile() {
    Token token;
    lexer.getToken(token);

    if (token.type == TokenTypeEnum::EOF_KW) {
        return;
    }else {
        throw std::runtime_error("Lexer::processEndOfFile()");
    }
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
    if (isMatching(baseTypesVector), true) {
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
    Token token;
    expectToken(IDENTIFIER);

    // AssignOrCallTail
    if (isMatching({EQ})) {
        Token token;
        lexer.getToken(token);
        processExpression();
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
    
}

void Parser::processWhileStatement() {}

void Parser::processReturnStatement(){}


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

}

void Parser::processBlock() {
    Token token;
    lexer.getToken(token);
    expectToken(TokenTypeEnum::LEFT_CURLY_PAREN);
    processStatementList();
    expectToken(TokenTypeEnum::RIGHT_CURLY_PAREN);
}

void Parser::processFunctionList() {
    Token token;
    expectToken(TokenTypeEnum::PUB_KW);
    expectToken(TokenTypeEnum::FN_KW);
    expectToken(TokenTypeEnum::IDENTIFIER);
    expectToken(TokenTypeEnum::LEFT_PAREN);
    //TODO: add implementation for parameters
    // processParameterList();
    expectToken(TokenTypeEnum::RIGHT_PAREN);
    processReturnType();
    processBlock();
}

void Parser::expectToken(TokenTypeEnum type) {
    Token token;
    lexer.getToken(token);
    if (token.type == type) {
        return;
    }else {
        throw std::runtime_error("Lexer::expectToken()");
    }
}


void Parser::createTree() {
    Token token;
    processImport();
    processFunctionList();
    processEndOfFile();
}


