

#ifndef PASCAL_S_TOKEN
#define PASCAL_S_TOKEN
#define KEYWORD_NUM 19

#include <cstdint>
#include <map>
#include "exception.h"
#include <stdexcept>
#include <vector>
#include <set>

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
    Nullptr,
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


using marker_type_underlying_type = uint8_t ;
enum class MarkerType : marker_type_underlying_type {
    Range = 0x00, // ..
    NEQ = 0x01, // <>
    LE = 0x02, // <=
    GE = 0x03, // >=
    LT = 0x04, // <
    EQ = 0x05, // =
    GT = 0x06, // >
    Add = 0x10, // +
    Sub = 0x11, // -
    Mod = 0x12, // %
    Mul = 0x20, // *
    Div = 0x21, // /

    LParen = 0x30, // (
    RParen = 0x31, // )
    LBracket = 0x40, // [
    RBracket = 0x41, // ]

    Assign = 0x50, // :=
    Comma = 0x51, // ,
    Dot = 0x52, // .
    Semicolon = 0x53, // ;
    Colon = 0x54, // :
};

marker_type_underlying_type  get_marker_pri(MarkerType marker_type);


struct Keyword : public Token {
    KeywordType key_type;

    explicit Keyword(KeywordType key_type) noexcept;
};

struct Marker : public Token {
    MarkerType marker_type;

    explicit Marker(MarkerType marker_type) noexcept;

    ~Marker();
};


struct ConstantString : public Token {
    const char *content;
    const char *attr;
};

struct ConstantReal : public Token {
    const char *content;
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
    char value;

    explicit ConstantChar(const char *content);
};

struct Identifier : public Token {
    const char *content;
    const char *attr;

    Identifier(const char *content);

    ~Identifier();
};

struct ConstantBoolean : public Token {
    const char *content;
    bool attr;

    ConstantBoolean(const char *content);

    ~ConstantBoolean();
};

void deleteToken(Token *pToken);

std::string convertToString(const Token *pToken);

#include <map>

using keyword_mapping = std::map<std::string, KeywordType>;
using reverse_keyword_mapping = std::map<KeywordType, const char *>;
extern keyword_mapping key_map;
extern reverse_keyword_mapping reverse_key_map;

using marker_mapping = std::map<std::string, MarkerType>;
using reverse_marker_mapping = std::map<MarkerType, const char *>;
extern marker_mapping marker_map;
extern reverse_marker_mapping reverse_marker_map;

const char *get_keyword_type_reversed(KeywordType kt);

const char *get_marker_type_reversed(MarkerType kt);


namespace predicate {
#define pascal_s_predicator(cls, cls_lower, lower, upper) bool is_ ## lower(const Token *tok);\
extern const cls cls_lower ##_## lower;

    pascal_s_predicator(Marker, marker, neq, NEQ)
    pascal_s_predicator(Marker, marker, le, LE)
    pascal_s_predicator(Marker, marker, ge, GE)
    pascal_s_predicator(Marker, marker, lt, LT)
    pascal_s_predicator(Marker, marker, eq, EQ)
    pascal_s_predicator(Marker, marker, gt, GT)
    pascal_s_predicator(Marker, marker, range, Range)

    pascal_s_predicator(Marker, marker, assgin, Assign)
    pascal_s_predicator(Marker, marker, add, Add)
    pascal_s_predicator(Marker, marker, sub, Sub)
    pascal_s_predicator(Marker, marker, mul, Mul)
    pascal_s_predicator(Marker, marker, div, Div)

    pascal_s_predicator(Marker, marker, mod, Mod)

    pascal_s_predicator(Marker, marker, lparen, LParen)
    pascal_s_predicator(Marker, marker, rparen, RParen)
    pascal_s_predicator(Marker, marker, lbracket, LBracket)
    pascal_s_predicator(Marker, marker, rbracket, RBracket)

    pascal_s_predicator(Marker, marker, comma, Comma)
    pascal_s_predicator(Marker, marker, dot, Dot)
    pascal_s_predicator(Marker, marker, semicolon, Semicolon)
    pascal_s_predicator(Marker, marker, colon, Colon)

    pascal_s_predicator(Keyword, keyword, program, Program)
    pascal_s_predicator(Keyword, keyword, const, Const)
    pascal_s_predicator(Keyword, keyword, var, Var)
    pascal_s_predicator(Keyword, keyword, procedure, Procedure)
    pascal_s_predicator(Keyword, keyword, function, Function)
    pascal_s_predicator(Keyword, keyword, begin, Begin)
    pascal_s_predicator(Keyword, keyword, end, End)

    pascal_s_predicator(Keyword, keyword, array, Array)
    pascal_s_predicator(Keyword, keyword, integer, Integer)
    pascal_s_predicator(Keyword, keyword, real, Real)
    pascal_s_predicator(Keyword, keyword, boolean, Boolean)
    pascal_s_predicator(Keyword, keyword, char, Char)

    pascal_s_predicator(Keyword, keyword, if, If)
    pascal_s_predicator(Keyword, keyword, then, Then)

    pascal_s_predicator(Keyword, keyword, else, Else)

    pascal_s_predicator(Keyword, keyword, for, For)

    pascal_s_predicator(Keyword, keyword, to, To)

    pascal_s_predicator(Keyword, keyword, do, Do)

    pascal_s_predicator(Keyword, keyword, of, Of)

#undef pascal_s_predicator


    bool token_equal(const Token *lhs, const Token *rhs);

    bool token_equal(const Token *lhs, const std::vector<Token *> *rhs);

    bool token_equal(const Token *lhs, const std::set<const Token *> *rhs);

}
#endif

