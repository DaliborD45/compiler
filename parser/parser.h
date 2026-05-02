//
// Created by Dalibor Detko on 17/04/2026.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H
#include "../lexer/lexer.h"
#include <vector>

#include "expression_parser/expression_parser.h"

class Parser {
    public:
        Lexer lexer;
        ExpressionParser expressionParser;
        explicit Parser(const Lexer& lexer) : lexer(lexer), expressionParser(lexer) {}

        void createTree();

    private:
        /**
         * Implemented with the strategy of precedence analysis,
         * so the implementation is not included within the LL grammar
         * Algorithm is derivated from precedence table
         */
        void processExpression();
        /**
         * Definitions of all functions, that are
         * architected based on LL grammar
         */
        void processImport();
        void processFunctionList();
        void processEndOfFile();
        void processReturnType();
        void processBlock();
        void processIfStatement();
        void processStatementList();
        void processWhileStatement();
        void processDeclarationStatement();
        void processAssignOrCallStatement();
        void processReturnStatement();
        void processParameterList();
        void processParameter();
        void processArgumentsList();

};
#endif //COMPILER_PARSER_H
