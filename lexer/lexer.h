//
// Created by Dalibor Detko on 15/04/2026.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <string>
#include <optional>
#include <istream>

/**
 * keywords for tokens
 */
typedef enum
{
    CONST_KW,             // 0
    IF_KW,                // 1
    ELSE_KW,              // 2
    FN_KW,                // 3
    I32_KW,               // 4
    F64_KW,               // 5
    NULL_KW,              // 6
    PUB_KW,               // 7
    RETURN_KW,            // 8
    U8_KW,                // 9
    VAR_KW,               // 10
    VOID_KW,              // 11
    WHILE_KW,             // 12
    PLUS,                 // 13
    MINUS,                // 14
    STAR,                 // 15
    SLASH,                // 16
    PERCENT,              // 17
    EQ,                   // 18
    DOUBLE_EQ,            // 19
    EXCLAMATION,          // 20
    DOT,                  // 21
    STRING,               // 22
    IDENTIFIER,           // 23
    NUMBER,               // 24
    FLOAT,                // 25
    EOF_KW,               // 26
    AT,                   // 27
    SEMICOLON,            // 28
    LEFT_PAREN,           // 29
    RIGHT_PAREN,          // 30
    LEFT_CURLY_PAREN,     // 31
    RIGHT_CURLY_PAREN,    // 32
    LEFT_SQUARE_PAREN,    // 33
    RIGHT_SQUARE_PAREN,   // 34
    NEW_LINE,             // 35
    TAB,                  // 36
    CARRIAGE_RETURN,      // 37
    COMMA,                // 38
    COLON,                // 39
    AT_IMPORT,            // 40
    QUESTION_MARK,        // 41
    GREATER_THAN_SIGN,    // 42
    LESS_THAN_SIGN,       // 43
    GREATER_EQ_THAN_SIGN, // 44
    LESS_EQ_THAN_SIGN,    // 45
    PIPE_SIGN,            // 46
    HASHTAG,              // 47
    NOT_EQ,               // 48
} TokenTypeEnum;

/**
 * Token structure
 */
typedef struct
{
    TokenTypeEnum type;
    std::string value;
} Token;


/**
 * Lexer is implemented with a greedy algorithm, that means,
 * it fetches as many characters as it can as long as the
 * sequence is valid for any type of Token, for example 10abc
 * would return number 10, identifier abc and than parser returns error
 * because number and identifier is not a valid sequence
 */
class Lexer {
public:
    explicit Lexer(std::istream& in);
    Token currentToken;
    int getToken(Token& tokenAddress);
    void returnToken(Token& token);
    /**
     * Checks whether the next token from Lexer is matching one of elements expected in @param typeArray,
     * @param shouldReturnTokenBack if set to false, the token will not be returned
     * back to the file
    */
    bool isMatching(const std::vector<TokenTypeEnum>& typeArray, bool shouldReturnTokenBack = true);
    /**
     * Takes next token from Lexer and compares match with the tokens in @param typeArray
     * if not matching, it raises error
     */
    void expectToken(const std::vector<TokenTypeEnum>& typeArray);
    void advance();

private:
    std::istream& input;
    //Used, when we are returning token back to the lexer (similar to peek)
    std::optional<Token> unresolvedToken = std::nullopt;
    int getCharacterFromInput();
    int peekCharacterFromInput();
    void skipWhitespaceAndComments();
    void skipComment();

    static bool isIdentifierStart(int ch);
    static bool isIdentifierPart(int ch);

    static TokenTypeEnum keywordCheck(const std::string& identifier);
    static Token createToken(const TokenTypeEnum& type, std::string value);

    std::string handleIdentifier(int firstChar);
    std::string handleNumber(int firstChar, bool& ok);
    std::string handleString();
    std::string handleMultilineString();
};
/**
 * Public lexer API
 */
int getToken(Token* tokenAddress);
void printErrorCode();

#endif // COMPILER_LEXER_H