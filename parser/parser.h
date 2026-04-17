//
// Created by Dalibor Detko on 17/04/2026.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H
#include "../lexer/lexer.h"

class Parser {
    public:
        Lexer lexer;
        explicit Parser(const Lexer& lexer) : lexer(lexer) {}
        void createTree();

    private:
        int processToken(Token& token);
        /**
         * Takes next token from Lexer and compares with expected @param type
         * if not matching, it raises error
         */
        void expectToken(TokenTypeEnum type);
        /**
         * Checks whether the next token from Lexer is matching one of elements expected in @param typeArray,
         * @param shouldReturnTokenBack if set to false, the token will not be returned
         * back to the file
        */
        bool isMatching(const std::vector<TokenTypeEnum>& typeArray, bool shouldReturnTokenBack);

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

        /**
         * Implemented with the strategy of precedence analysis,
         * so the implementation is not included within the LL grammar
         */
        void processExpression();
};
#endif //COMPILER_PARSER_H
