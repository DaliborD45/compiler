//
// Created by Dalibor Detko on 17/04/2026.
//

#include "parser.h"
#include "../helpers/helpers.h"
#include "../lexer/lexer.h"


void Parser::processExpression() {
    expressionParser.parseExpression(0);
}

void Parser::processImport() {
    //for now, we demand the import of the core library for every program
    lexer.expectToken({CONST_KW});
    lexer.expectToken({IDENTIFIER});
    lexer.expectToken({EQ});
    lexer.expectToken({AT_IMPORT});
    lexer.expectToken({LEFT_PAREN});
    lexer.expectToken({STRING});
    lexer.expectToken({RIGHT_PAREN});
    lexer.expectToken({SEMICOLON});
}

void Parser::processEndOfFile() {
   lexer.expectToken({EOF_KW});
}

void Parser::processReturnType() {
    //parse optional type
    if (lexer.isMatching({QUESTION_MARK})) {
        lexer.advance();
    }
    //parse base type
    if (lexer.isMatching(baseTypesVector)) {
        lexer.advance();
    }
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



void Parser::processParameter() {
    lexer.expectToken({IDENTIFIER});
    lexer.expectToken({COLON});
    lexer.expectToken(baseTypesVector);

    // parametersTail
    if (lexer.isMatching({COMMA})) {
        lexer.advance();
        processParameter();
    }
}

void Parser::processParameterList() {
    if (lexer.isMatching({IDENTIFIER})) {
        processParameter();
    }
}

void Parser::processFunctionList() {
    if (!lexer.isMatching({PUB_KW})) {
        return;
    }
    lexer.expectToken({PUB_KW});
    lexer.expectToken({FN_KW});
    lexer.expectToken({IDENTIFIER});
    lexer.expectToken({LEFT_PAREN});
    processParameterList();
    lexer.expectToken({RIGHT_PAREN});
    processReturnType();
    processBlock();

    // recursively call again
    processFunctionList();
}


void Parser::createTree() {
    processImport();
    processFunctionList();
    processEndOfFile();
}




