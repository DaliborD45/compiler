//
// Created by Dalibor Detko on 17/04/2026.
//

#include "parser.h"
#include <stack>
#include <iostream>
#include "../helpers/helpers.h"
#include "../precedenceAnalysisStack/precedenceAnalysisStack.h"
#include "../lexer/lexer.h"



void Parser::processExpression() {
    lexer.getToken(expressionParser.currentToken);
    expressionParser.parseExpression(0);
    //This technique can   be used, only because this language does not support function calls in the expressions
    // if (isMatching({IDENTIFIER})) {
    //     lexer.getToken(token);
    //     if (isMatching({LEFT_PAREN})) {
    //         lexer.getToken(token);
    //         processArgumentsList();
    //         expectToken({RIGHT_PAREN});
    //         // expectToken({SEMICOLON});
    //         return;
    //     }
    // }

    //No expression to evaluate, just return
    // if (isMatching({SEMICOLON})) {
    //     return;
    // }
    //
    // if (token.type != IDENTIFIER) {
    //     lexer.getToken(token);
    // }
    //
    // PrecAnalysisStack precAnalysisStack;
    // // for the purposes of algorithm, the $ means the beginning of the expression
    // precAnalysisStack.push(DOLLAR_TOKEN);
    //
    // while (!precAnalysisStack.empty()) {
    //     Token tokenInStack = precAnalysisStack.getTopTerminal();
    //     std::string precedence = checkPrecedenceOfTokens(tokenInStack, token);
    //     //Token that came from lexer has bigger precedence
    //     if (precedence == "<" or precedence == "=") {
    //         precAnalysisStack.push(token);
    //         lexer.getToken(token);
    //
    //         //This means that it is not part of the expression, so we just return it and end analysis
    //         if (!includes(allowedTokenTypes, token.type)) {
    //             lexer.returnToken(token);
    //             token = DOLLAR_TOKEN;
    //         }
    //
    //     }
    //     if (precedence == ">") {
    //         // do reduction
    //         precAnalysisStack.reduceStack();
    //     }
    //     /**
    //      * this means comparision of $ with $, that means that the stack should be empty now and loop should end
    //      * by the logic, on the tokenStack, there should be exactly on AST node covering full expresion
    //      * or just one non-terminal
    //      */
    //     if (precedence == "end") {
    //         //Here we should do something with our expression in stack
    //         return;
    //     }
    // }
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
    Token token;
    //parse optional type
    if (lexer.isMatching({QUESTION_MARK})) {
        lexer.getToken(token);
    }
    //parse base type
    if (lexer.isMatching(baseTypesVector)) {
        lexer.getToken(token);
    }
}


void Parser::processDeclarationStatement() {
    lexer.expectToken({IDENTIFIER});
    //Process declaration tail
    if (lexer.isMatching({COLON})) {
        Token token;
        lexer.getToken(token);
        lexer.expectToken(baseTypesVector);
    }

    //process after tail
    lexer.expectToken({EQ});
    processExpression();
    // lexer.expectToken({SEMICOLON});

}

//TODO: implement argument list
void Parser::processArgumentsList() {
    Token token;
    processExpression();
    if (lexer.isMatching({COMMA})) {
        lexer.getToken(token);
        processExpression();
    }
}

void Parser::processAssignOrCallStatement() {
    // AssignOrCallTail
    if (lexer.isMatching({EQ})) {
        Token token;
        lexer.getToken(token);
        processExpression();
        // lexer.expectToken({SEMICOLON});
    }
    else {
        lexer.expectToken({LEFT_PAREN});
        processArgumentsList();
        lexer.expectToken({RIGHT_PAREN});
        lexer.expectToken({SEMICOLON});
    }
}

void Parser::processIfStatement() {
    //processExpression is also handling brackets
    processExpression();
    processBlock();

    //process else statement
    if (lexer.isMatching({ELSE_KW})) {
        Token token;
        lexer.getToken(token);
        processBlock();
    }
    //else epsilon
}

void Parser::processWhileStatement() {
    //processExpression is also handling brackets
    processExpression();
    processBlock();
    if (lexer.isMatching({ELSE_KW})) {
        Token token;
        lexer.getToken(token);
        processBlock();
    }
    //else epsilon
}

void Parser::processReturnStatement() {
    //returnTail
    if (lexer.isMatching({SEMICOLON})) {
        Token token;
        lexer.getToken(token);
        return;
    }else {
        processExpression();
        // lexer.expectToken({SEMICOLON});

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
        Token token;
        lexer.getToken(token);
        processParameter();
    }
}

void Parser::processParameterList() {
    if (lexer.isMatching({IDENTIFIER})) {
        processParameter();
    }
}

void Parser::processFunctionList() {
    Token token;
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
    Token token;
    processImport();
    processFunctionList();
    processEndOfFile();
}




