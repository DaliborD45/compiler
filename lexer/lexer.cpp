#include "lexer.h"
#include "../errors.h"
#include "../helpers/helpers.h"
#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>

int LEXER_ERROR_CODE = 0;

Lexer::Lexer(std::istream& in) : input(in) {}

void Lexer::returnToken(Token& token) {
    if (unresolvedToken.has_value()) {
        throw std::runtime_error("You cannot return token without getToken function");
    }
    unresolvedToken = token;
}

void Lexer::advance() {
   getToken(currentToken);
}

bool Lexer::isMatching(const std::vector<TokenTypeEnum>& typeArray, bool shouldReturnTokenBack){
    Token token;
    getToken(token);
    bool is_matching = false;
    if (includes(typeArray, token.type)) {
        is_matching = true;
    }
    if (shouldReturnTokenBack) {
        returnToken(token);
    }
    return is_matching;
}


void Lexer::expectToken(const std::vector<TokenTypeEnum>& typeArray) {
    advance();
    if (includes(typeArray, currentToken.type)) {
        return;
    }else {
        // Here we are mixing stdout with stderr, mby refactor
        printTokenObject(currentToken);
        printVector(typeArray);
        throw std::runtime_error("Program expected tokens above, but got " + getTokenTypeName(currentToken.type) + " instead");
    }
}

int Lexer::getToken(Token& tokenAddress)
{
    if (unresolvedToken.has_value()) {
        tokenAddress = unresolvedToken.value();
        unresolvedToken = std::nullopt;
        return 0;
    }
    skipWhitespaceAndComments();

    const int character = getCharacterFromInput();

    switch (character)
    {
        case EOF:
            tokenAddress = createToken(EOF_KW, "");
            return 0;

        case ':':
            tokenAddress = createToken(COLON, ":");
            return 0;

        case ',':
            tokenAddress = createToken(COMMA, ",");
            return 0;

        case ';':
            tokenAddress = createToken(SEMICOLON, ";");
            return 0;

        case '.':
            tokenAddress = createToken(DOT, ".");
            return 0;

        case '?':
            tokenAddress = createToken(QUESTION_MARK, "?");
            return 0;

        case '+':
            tokenAddress = createToken(PLUS, "+");
            return 0;

        case '*':
            tokenAddress = createToken(STAR, "*");
            return 0;

        case '-':
            tokenAddress = createToken(MINUS, "-");
            return 0;

        case '%':
            tokenAddress = createToken(PERCENT, "%");
            return 0;

        case '(':
            tokenAddress = createToken(LEFT_PAREN, "(");
            return 0;

        case ')':
            tokenAddress = createToken(RIGHT_PAREN, ")");
            return 0;

        case '{':
            tokenAddress = createToken(LEFT_CURLY_PAREN, "{");
            return 0;

        case '}':
            tokenAddress = createToken(RIGHT_CURLY_PAREN, "}");
            return 0;

        //TODO: here we have to also make sure we return u8_kw as whole
        case '[':
            tokenAddress = createToken(LEFT_SQUARE_PAREN, "[");
            return 0;

        case ']':
            tokenAddress = createToken(RIGHT_SQUARE_PAREN, "]");
            return 0;

        case '|':
            tokenAddress = createToken(PIPE_SIGN, "|");
            return 0;

        case '/':
            tokenAddress = createToken(SLASH, "/");
            return 0;

        case '=':
            if (peekCharacterFromInput() == '=')
            {
                getCharacterFromInput();
                tokenAddress = createToken(DOUBLE_EQ, "==");
            }
            else
            {
                tokenAddress = createToken(EQ, "=");
            }
            return 0;

        case '!':
            if (peekCharacterFromInput() == '=')
            {
                getCharacterFromInput();
                tokenAddress = createToken(NOT_EQ, "!=");
            }
            else
            {
                tokenAddress = createToken(EXCLAMATION, "!");
            }
            return 0;

        case '>':
            if (peekCharacterFromInput() == '=')
            {
                getCharacterFromInput();
                tokenAddress = createToken(GREATER_EQ_THAN_SIGN, ">=");
            }
            else
            {
                tokenAddress = createToken(GREATER_THAN_SIGN, ">");
            }
            return 0;

        case '<':
            if (peekCharacterFromInput() == '=')
            {
                getCharacterFromInput();
                tokenAddress = createToken(LESS_EQ_THAN_SIGN, "<=");
            }
            else
            {
                tokenAddress = createToken(LESS_THAN_SIGN, "<");
            }
            return 0;

        case '@':
        {
            if (peekCharacterFromInput() == 'i')
            {
                int first = getCharacterFromInput();
                std::string identifier = handleIdentifier(first);

                if (identifier == "import")
                {
                    tokenAddress = createToken(AT_IMPORT, "@import");
                    return 0;
                }

                LEXER_ERROR_CODE = 3;
                return LEXER_ERR;
            }

            tokenAddress = createToken(AT, "@");
            return 0;
        }

        case '"':
        {
            try {
                std::string value = handleString();
                tokenAddress = createToken(STRING, value);
            } catch (...) {
                LEXER_ERROR_CODE = 1;
                return LEXER_ERR;
            }
            return 0;
        }

        case '\\':
        {
            if (peekCharacterFromInput() == '\\')
            {
                getCharacterFromInput(); // consume second '\'

                try {
                    std::string value = handleMultilineString();
                    tokenAddress = createToken(STRING, value);
                } catch (...) {
                    LEXER_ERROR_CODE = 1;
                    return LEXER_ERR;
                }
                return 0;
            }

            LEXER_ERROR_CODE = 1;
            return LEXER_ERR;
        }

        default:
            break;
    }

    if (isIdentifierStart(character))
    {
        std::string identifier = handleIdentifier(character);
        TokenTypeEnum tokenType = keywordCheck(identifier);
        tokenAddress = createToken(tokenType, identifier);
        return 0;
    }

    if (std::isdigit(static_cast<unsigned char>(character)))
    {
        bool ok = true;
        std::string number = handleNumber(character, ok);

        if (!ok)
        {
            LEXER_ERROR_CODE = 2;
            return LEXER_ERR;
        }

        if (number.find('.') != std::string::npos ||
            number.find('e') != std::string::npos ||
            number.find('E') != std::string::npos)
        {
            tokenAddress = createToken(FLOAT, number);
        }
        else
        {
            tokenAddress = createToken(NUMBER, number);
        }

        return 0;
    }

    LEXER_ERROR_CODE = 4;
    return LEXER_ERR;
}

int Lexer::getCharacterFromInput()
{
    return input.get();
}

int Lexer::peekCharacterFromInput()
{
    return input.peek();
}

void Lexer::skipWhitespaceAndComments()
{
    while (true)
    {
        int ch = peekCharacterFromInput();

        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r')
        {
            getCharacterFromInput();
            continue;
        }

        if (ch == '#')
        {
            skipComment();
            continue;
        }

        if (ch == '/')
        {
            getCharacterFromInput(); // consume '/'

            if (peekCharacterFromInput() == '/')
            {
                getCharacterFromInput(); // consume second '/'
                skipComment();
                continue;
            }

            // not a comment -> put meaning back by storing slash nowhere impossible with peek approach,
            // so easiest is to push it back via stream
            input.putback('/');
            break;
        }

        break;
    }
}

void Lexer::skipComment()
{
    while (true)
    {
        int ch = getCharacterFromInput();
        if (ch == '\n' || ch == EOF)
        {
            return;
        }
    }
}

bool Lexer::isIdentifierStart(const int ch)
{
    return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
}

bool Lexer::isIdentifierPart(const int ch)
{
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

TokenTypeEnum Lexer::keywordCheck(const std::string& identifier)
{
    static const std::unordered_map<std::string, TokenTypeEnum> keywords = {
        {"const", CONST_KW},
        {"if", IF_KW},
        {"else", ELSE_KW},
        {"fn", FN_KW},
        {"i32", I32_KW},
        {"f64", F64_KW},
        {"null", NULL_KW},
        {"pub", PUB_KW},
        {"u8", U8_KW},
        {"var", VAR_KW},
        {"void", VOID_KW},
        {"while", WHILE_KW},
        {"return", RETURN_KW},
    };

    auto it = keywords.find(identifier);
    if (it != keywords.end())
    {
        return it->second;
    }

    return IDENTIFIER;
}

Token Lexer::createToken(const TokenTypeEnum& type, std::string value)
{
    return Token{type, std::move(value)};
}

std::string Lexer::handleIdentifier(const int firstChar)
{
    std::string identifier;
    identifier.push_back(static_cast<char>(firstChar));

    while (true)
    {
        int ch = peekCharacterFromInput();
        if (!isIdentifierPart(ch))
        {
            break;
        }

        identifier.push_back(static_cast<char>(getCharacterFromInput()));
    }

    return identifier;
}

std::string Lexer::handleNumber(const int firstChar, bool& ok)
{
    std::string number;
    ok = true;
    number.push_back(static_cast<char>(firstChar));

    while (std::isdigit(static_cast<unsigned char>(peekCharacterFromInput())))
    {
        number.push_back(static_cast<char>(getCharacterFromInput()));
    }

    if (peekCharacterFromInput() == '.')
    {
        number.push_back(static_cast<char>(getCharacterFromInput()));

        if (!std::isdigit(static_cast<unsigned char>(peekCharacterFromInput())))
        {
            ok = false;
            return "";
        }

        while (std::isdigit(static_cast<unsigned char>(peekCharacterFromInput())))
        {
            number.push_back(static_cast<char>(getCharacterFromInput()));
        }
    }

    int ch = peekCharacterFromInput();
    if (ch == 'e' || ch == 'E')
    {
        number.push_back(static_cast<char>(getCharacterFromInput()));

        ch = peekCharacterFromInput();
        if (ch == '+' || ch == '-')
        {
            number.push_back(static_cast<char>(getCharacterFromInput()));
        }

        if (!std::isdigit(static_cast<unsigned char>(peekCharacterFromInput())))
        {
            ok = false;
            return "";
        }

        while (std::isdigit(static_cast<unsigned char>(peekCharacterFromInput())))
        {
            number.push_back(static_cast<char>(getCharacterFromInput()));
        }
    }

    return number;
}

std::string Lexer::handleString()
{
    std::string result;

    while (true)
    {
        int ch = getCharacterFromInput();

        if (ch == EOF)
        {
            throw std::runtime_error("Lexer::handleString()");
        }

        if (ch == '"')
        {
            break;
        }

        if (ch == '\\')
        {
            int next = getCharacterFromInput();

            if (next == EOF)
            {
                throw std::runtime_error("Lexer::handleString()");
            }

            if (next != '"' && next != '\\' && next != 'n' && next != 't' && next != 'x')
            {
                throw std::runtime_error("Lexer::handleString()");
            }

            result.push_back('\\');
            result.push_back(static_cast<char>(next));
            continue;
        }

        result.push_back(static_cast<char>(ch));
    }

    return result;
}

std::string Lexer::handleMultilineString()
{
    std::string result;
    while (true)
    {
        result += "\\\\";

        int ch = getCharacterFromInput();
        while (ch != '\n' && ch != EOF)
        {
            result.push_back(static_cast<char>(ch));
            ch = getCharacterFromInput();
        }

        if (ch == '\n')
        {
            result.push_back('\n');
        }

        while (true)
        {
            int next = peekCharacterFromInput();
            if (next == ' ' || next == '\t' || next == '\r' || next == '\n')
            {
                getCharacterFromInput();
            }
            else
            {
                break;
            }
        }

        if (peekCharacterFromInput() != '\\')
        {
            break;
        }

        getCharacterFromInput(); // first '\'
        if (peekCharacterFromInput() != '\\')
        {
            input.putback('\\');
            break;
        }
        getCharacterFromInput(); // second '\'
    }

    if (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }

    return result;
}

/**
 * Helper functions
 */


void printErrorCode()
{
    switch (LEXER_ERROR_CODE)
    {
        case 1:
            std::cerr << "There was an error with string\n";
            break;
        case 2:
            std::cerr << "There was an error with number\n";
            break;
        case 3:
            std::cerr << "There was an error with @ directive\n";
            break;
        case 4:
            std::cerr << "There was an unknown lexer error\n";
            break;
        default:
            std::cerr << "There was an error that is not specified\n";
            break;
    }
}
