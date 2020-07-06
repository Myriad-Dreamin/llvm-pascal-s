
#ifndef PASCALS_STD_EXCEPTION
#define PASCALS_STD_EXCEPTION

#include <exception>
#include <string>
#include <utility>
#include "lib/stdtype.h"

struct Token;
enum class TokenType : pascal_s::token_type_underlying_type;

namespace ast {
    struct Node;
}

class RuntimeReinterpretTokenException : public std::exception {
    const Token *token;
    std::string msg;
public:
    explicit RuntimeReinterpretTokenException(const Token *token);

    [[nodiscard]] const char *what() const noexcept override;
};

class RuntimeReinterpretASTException : public std::exception {
    const ast::Node *node;
    std::string msg;
public:
    explicit RuntimeReinterpretASTException(const ast::Node *token);

    [[nodiscard]] const char *what() const noexcept override;
};

enum class PascalSErrno : pascal_s::errno_t {
    NoError,
    ParseError,
    ParseExpectVGotError,
    ParseExpectTGotError,
    ParseExpectSGotError,
    SemanticError,
};

struct PascalSError : public std::exception {
    // 0 ~ 8字节
    PascalSErrno type;
    pascal_s::line_t line;
    // 8 ~ 16字节
    pascal_s::column_t column;
    pascal_s::length_t length;
    // 16 ~ 24字节
    pascal_s::offset_t offset;
    std::string msg;

    explicit PascalSError(std::string msg, PascalSErrno t) : msg(std::move(msg)), type(t) {}

    ~PascalSError() override = default;

    [[nodiscard]] const char *what() const noexcept override {
        return msg.c_str();
    };
};

struct PascalSParseError : public PascalSError {
    std::string fn;

    explicit PascalSParseError(const char *fn, std::string msg, PascalSErrno t = PascalSErrno::ParseError) :
            PascalSError(std::move(msg), t), fn(fn) {}

    ~PascalSParseError() override = default;
};

struct PascalSSemanticError : public PascalSError {
    std::string fn;

    explicit PascalSSemanticError(const char *fn, const pascal_s::Pos *problem_pos, std::string msg,
                                  PascalSErrno t = PascalSErrno::SemanticError);

    ~PascalSSemanticError() override = default;
};

struct PascalSParseExpectVGotError : public PascalSParseError {
    const Token *expected, *got;

    PascalSParseExpectVGotError(
            const char *fn, const Token *expected, const Token *got, std::string msg = "");

    ~PascalSParseExpectVGotError() override = default;
};

struct PascalSParseExpectTGotError : public PascalSParseError {
    const Token *got;
    pascal_s::token_type_underlying_type expected;

    PascalSParseExpectTGotError(const char *fn, TokenType expected, const Token *got,
                                std::string msg = "");

    ~PascalSParseExpectTGotError() override = default;
};

struct PascalSParseExpectSGotError : public PascalSParseError {
    const Token *got;
    const char *expected;

    PascalSParseExpectSGotError(const char *fn, const char *expected, const Token *got,
                                std::string msg = "");

    ~PascalSParseExpectSGotError() override = default;
};

#endif
