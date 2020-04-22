

#ifndef PASCAL_S_TOKEN
#define PASCAL_S_TOKEN

#include <stdint.h>

struct LexerInfo {
    int64_t row, column;
};

enum class TokenType  {
    Keyword,
    ConstantString,
    ConstantChar,
    ConstantFloat,
    ConstantInteger,
    ConstantBoolean,
};

struct Token {
    TokenType type;
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

//    Div,
//    Mod,
//    And,
//    Or,
//    Not,
};

struct Keyword: public Token {
    KeywordType key_type;
};

struct ConstantString: public Token {
    const char* content;
};

struct ConstantReal: public Token {
    const char* content;
};

struct ConstantInteger: public Token {
    const char* content;
};

struct ConstantChar: public Token {
    const char* content;
};

#endif

