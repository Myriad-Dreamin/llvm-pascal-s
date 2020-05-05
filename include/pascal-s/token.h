

#ifndef PASCAL_S_TOKEN
#define PASCAL_S_TOKEN
#define KEYWORD_NUM 19

#include <stdint.h>
#include <map>
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
    Identifier,
    Marker,
    Length
};

using line_t = uint64_t;
using column_t = uint64_t;

struct Token {
    TokenType type;
    //todo: add line, column info
    line_t line;
    column_t column;
};

enum class KeywordType {
    Program ,
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
    const char* attr;

    explicit Keyword(KeywordType key_type) : Token(), key_type(key_type) {
        this->type = TokenType::Keyword;
    }
};

struct ConstantString: public Token {
    const char* content;
    const char* attr;
};

struct ConstantReal: public Token {
    const char* content;
    double attr;

    ConstantReal(const char *content);

    ~ConstantReal();
};

struct ConstantInteger : public Token {
    const char *content;
    int64_t attr;

    ConstantInteger(const char *content);

    ~ConstantInteger();
};

struct ConstantChar : public Token {
    const char *content;
    const char* attr;

    ConstantChar(const char *content);

    ~ConstantChar();
};

struct Identifier : public Token {
    const char *content;
    const char* attr;

    Identifier(const char *content);

    ~Identifier();
};

struct ConstantBoolean : public Token {
    const char *content;
    bool attr;

    ConstantBoolean(const char *content);

    ~ConstantBoolean();
};

struct Marker : public Token {
    const char *content;
    const char *attr;

    Marker(const char *content);

    ~Marker();
};

void deleteToken(Token *pToken);

std::string convertToString(const Token *pToken);

#include <map>

using keyword_mapping = std::map<std::string, KeywordType>;
using reverse_keyword_mapping = std::map<KeywordType, const char *>;
extern keyword_mapping key_map;
extern reverse_keyword_mapping reverse_key_map;

const char *get_keyword_type_reversed(KeywordType kt);

#endif

