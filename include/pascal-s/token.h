

#ifndef PASCAL_S_TOKEN
#define PASCAL_S_TOKEN

#include <map>
#include <set>
#include <vector>
#include <cassert>
#include <string>
#include "exception.h"
#include "lib/stdtype.h"

enum class TokenType : pascal_s::token_type_underlying_type {
    Unknown = 0,
    ConstantString = 2,
    ConstantChar = 3,
    ConstantReal = 4,
    ConstantInteger = 5,
    ConstantBoolean = 6,
    Identifier = 7,
    Nullptr = 9,
    ErrorToken = 10,
    Comment = 11,
    Length = 12,
    ConstRangeL = ConstantString,
    ConstRangeR = ConstantBoolean,

    MarkerRangeL = 0x100,
    MarkerRangeR = 0x200,

    KeywordRangeL = 0x200,
    KeywordRangeR = 0x300,

    Range = MarkerRangeL + 0x00, // ..
    LogicNot = MarkerRangeL + 0x01, // not

    NEQ = MarkerRangeL + 0x11, // <>
    LE = MarkerRangeL + 0x12, // <=
    GE = MarkerRangeL + 0x13, // >=
    LT = MarkerRangeL + 0x14, // <
    EQ = MarkerRangeL + 0x15, // =
    GT = MarkerRangeL + 0x16, // >

    Add = MarkerRangeL + 0x20, // +
    Sub = MarkerRangeL + 0x21, // -
    LogicOr = MarkerRangeL + 0x22, // or

    Mul = MarkerRangeL + 0x30, // *
    Div = MarkerRangeL + 0x31, // /
    Mod = MarkerRangeL + 0x32, // mod
    LogicAnd = MarkerRangeL + 0x33, // and

    LParen = MarkerRangeL + 0x40, // (
    RParen = MarkerRangeL + 0x41, // )
    LBracket = MarkerRangeL + 0x50, // [
    RBracket = MarkerRangeL + 0x51, // ]

    Assign = MarkerRangeL + 0x60, // :=
    Comma = MarkerRangeL + 0x61, // ,
    Dot = MarkerRangeL + 0x62, // .
    Semicolon = MarkerRangeL + 0x63, // ;
    Colon = MarkerRangeL + 0x64, // :

    Program = KeywordRangeL + 0x00,
    Const = KeywordRangeL + 0x01,
    Var = KeywordRangeL + 0x02,
    Procedure = KeywordRangeL + 0x03,
    Function = KeywordRangeL + 0x04,
    Begin = KeywordRangeL + 0x05,
    End = KeywordRangeL + 0x06,

    Array = KeywordRangeL + 0x07,
    Integer = KeywordRangeL + 0x08,
    Real = KeywordRangeL + 0x09,
    Boolean = KeywordRangeL + 0x0a,
    Char = KeywordRangeL + 0x0b,

    If = KeywordRangeL + 0x0c,
    Then = KeywordRangeL + 0x0d,
    Else = KeywordRangeL + 0x0e,
    For = KeywordRangeL + 0x0f,
    To = KeywordRangeL + 0x10,
    Do = KeywordRangeL + 0x11,
    Of = KeywordRangeL + 0x12,

    Read = KeywordRangeL + 0x13,
    Write = KeywordRangeL + 0x14
};


struct Token : pascal_s::Pos {
    // 0 ~ 8字节
    TokenType type = TokenType::Unknown;

    Token() noexcept = default;

    explicit Token(TokenType type) noexcept: type(type) {}

    [[nodiscard]] const pascal_s::Pos *visit_pos() const {
        return reinterpret_cast<const pascal_s::Pos *>(this);
    }

    pascal_s::Pos *visit_pos() {
        return reinterpret_cast<pascal_s::Pos *>(this);
    }
};


struct ErrorToken : public Token {
    const char *content;
    const char *hint;

    explicit ErrorToken(const char *content, const char *hint = nullptr);

    explicit ErrorToken(const char *content, int len, const char *hint = nullptr);

    static ErrorToken *copy_in(const char *content, const char *hint = nullptr);

    ~ErrorToken();
};

using MarkerType = TokenType;
using KeywordType = TokenType;
using Keyword = Token;
using Marker = Token;

struct ConstantString : public Token {
    const char *attr = nullptr;
};

struct Comment : public Token {
    const char *content;

    explicit Comment(const char *content);
};

struct ConstantReal : public Token {
    const char *content;
    pascal_s::pascal_s_real_t attr;

    ConstantReal(const char *content, double attr);

    ~ConstantReal();
};

struct ConstantInteger : public Token {
    pascal_s::pascal_s_integer_t attr;

    explicit ConstantInteger(pascal_s::pascal_s_integer_t attr);

    ~ConstantInteger();
};

struct ConstantChar : public Token {
    char attr;

    explicit ConstantChar(char ch);

    ~ConstantChar();
};

struct Identifier : public Token {
    const char *content;

    explicit Identifier(const char *content);

    ~Identifier();
};

struct ConstantBoolean : public Token {
    bool attr;

    explicit ConstantBoolean(bool attr);

    ~ConstantBoolean();
};

void deleteToken(Token *pToken);

std::string convertToString(const Token *pToken);

const char *convertToString(TokenType tt);

KeywordType get_keyword_type(const std::string &kt);

const std::string &get_keyword_type_reversed(KeywordType kt);

pascal_s::marker_type_underlying_type get_marker_pri(MarkerType);

MarkerType get_marker_type(const std::string &mt);

const std::string &get_marker_type_reversed(MarkerType mt);

namespace predicate {
    bool is_const_token(TokenType tt);

    bool is_binary_sign(MarkerType mt);

    bool is_binary_sign(const Token *t);

    [[maybe_unused]] bool is_marker(const Token *t);

    [[maybe_unused]] bool is_marker_type(MarkerType t);

    [[maybe_unused]] bool is_keyword(const Token *t);

    [[maybe_unused]] bool is_keyword_type(MarkerType t);

#define pascal_s_predicator(cls, cls_lower, lower, upper) bool is_ ## lower(const Token *tok);\
extern const cls cls_lower ##_## lower;

    pascal_s_predicator(Marker, marker, logic_not, LogicNot)

    pascal_s_predicator(Marker, marker, logic_and, LogicAnd)

    pascal_s_predicator(Marker, marker, logic_or, LogicOr)

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

    pascal_s_predicator(Keyword, keyword, read, Read)

    pascal_s_predicator(Keyword, keyword, write, Write)

#undef pascal_s_predicator


    bool token_equal(const Token *lhs, const Token *rhs);

    [[maybe_unused]] bool token_equal(const Token *lhs, const std::vector<Token *> *rhs);

    bool token_equal(const Token *lhs, const std::set<const Token *> *rhs);

}


#endif
