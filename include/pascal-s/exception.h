
#ifndef PASCALS_STD_EXCEPTION
#define PASCALS_STD_EXCEPTION

#include <exception>
#include <string>
#include <utility>

struct Token;
enum class TokenType;
using token_type_underlying_type =  uint16_t;

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

struct PascalSError : public std::exception {
    std::string msg;
    explicit PascalSError(std::string msg) : msg(std::move(msg)) {}
    ~PascalSError() override = default;

    [[nodiscard]] const char *what() const noexcept override {
        return msg.c_str();
    };
};

struct PascalSParseError : public PascalSError {
    std::string fn;

    explicit PascalSParseError(char * fn, std::string msg) : PascalSError(std::move(msg)), fn(fn) {}
    ~PascalSParseError() override = default;
};

struct PascalSParseExpectGotError : public PascalSParseError {
    const Token *expected, *got;

    PascalSParseExpectGotError(char * fn, const Token * expected, const Token *got,
                               std::string msg="") : PascalSParseError(fn, std::move(msg)), expected(expected), got(got) {}
    ~PascalSParseExpectGotError() override = default;
};

struct PascalSParseExpectTGotError : public PascalSParseError {
    const Token *got;
    token_type_underlying_type expected;
    PascalSParseExpectTGotError(char * fn, TokenType expected, const Token *got,
                                std::string msg="");
    ~PascalSParseExpectTGotError() override = default;
};

struct PascalSParseExpectSGotError : public PascalSParseError {
    const Token *got;
    const char * expected;
    PascalSParseExpectSGotError(char * fn, const char* expected, const Token *got,
                                std::string msg="");
    ~PascalSParseExpectSGotError() override = default;
};

#endif

