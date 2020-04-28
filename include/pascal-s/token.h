

#ifndef PASCAL_S_TOKEN
#define PASCAL_S_TOKEN

#include <stdint.h>
#include "exception.h"

struct LexerInfo {
    int64_t row, column;
};

enum class TokenType {
    Keyword,
    ConstantString,
    ConstantChar,
    ConstantReal,
    ConstantInteger,
    ConstantBoolean,

    Length
};

using line_t = uint64_t;
using column_t = uint64_t;

struct Token {
    TokenType type;
    //todo: add line, column info
//    line_t line;
//    column_t column;
};

enum class KeywordType {
    Program,
    Const,
    Var,
    Procedure,
    Function,
    Begin,
    End,

    Array,
    Integer,
    Real,
    Boolean,
    Char,

    If,
    Then,
    Else,
    For,
    To,
    Do,
    Of,

    //todo: keyword type
//    Div,
//    Mod,
//    And,
//    Or,
//    Not,

    Length
};

struct Keyword : public Token {
    KeywordType key_type;

    explicit Keyword(KeywordType key_type) : Token(), key_type(key_type) {
        this->type = TokenType::Keyword;
    }
};

struct ConstantString: public Token {
    const char* content;
};

struct ConstantReal: public Token {
    const char* content;
};

struct ConstantInteger : public Token {
    const char *content;
};

struct ConstantChar : public Token {
    const char *content;
};


void deleteToken(Token *pToken);

#endif

