

#include <fmt/core.h>
#include <pascal-s/exception.h>
#include <pascal-s/token.h>
#include <pascal-s/interface.h>
#include <pascal-s/llvm-ast.h>


RuntimeReinterpretTokenException::RuntimeReinterpretTokenException(const Token *token)
        : token(token), msg(fmt::format("runtime reinterpret token error: unknown type: {}", token->type)) {
}

const char *RuntimeReinterpretTokenException::what() const noexcept {
    return msg.c_str();
}

RuntimeReinterpretASTException::RuntimeReinterpretASTException(const ast::Node *node)
        : node(node), msg(fmt::format("runtime reinterpret node error: unknown type: {}", node->type)) {

}

const char *RuntimeReinterpretASTException::what() const noexcept {
    return msg.c_str();
}

PascalSParseExpectVGotError::PascalSParseExpectVGotError(const char *fn, const Token *expected, const Token *got,
                                                         std::string msg) :
        PascalSParseError(fn, fmt::format("{} parse failed: "
                                          "expected: {}, "
                                          "got: {}", fn, convertToString(expected), convertToString(got)),
                          PascalSErrno::ParseExpectVGotError), expected(expected), got(got) {
    if (!msg.empty()) {
        this->msg += ":" + msg;
    }
    if (got) {
        this->length = got->length;
        this->offset = got->offset;
        this->line = got->line;
        this->column = got->column;
    } else {
        this->length = 0;
        this->offset = 0;
        this->line = 1;
        this->column = 0;
    }
}

PascalSParseExpectTGotError::PascalSParseExpectTGotError(const char *fn, TokenType expected, const Token *got,
                                                         std::string msg) :
        PascalSParseError(fn,
                          fmt::format("{} parse failed: "
                                      "expected type: {}, "
                                      "got: {}", fn, convertToString(expected), convertToString(got)),
                          PascalSErrno::ParseExpectTGotError),
        expected(static_cast<pascal_s::token_type_underlying_type>(expected)), got(got) {
    if (!msg.empty()) {
        this->msg += ":" + msg;
    }
    if (got) {
        this->length = got->length;
        this->offset = got->offset;
        this->line = got->line;
        this->column = got->column;
    } else {
        this->length = 0;
        this->offset = 0;
        this->line = 1;
        this->column = 0;
    }
}

PascalSParseExpectSGotError::PascalSParseExpectSGotError(
        const char *fn, const char *expected, const Token *got, std::string msg) :
        PascalSParseError(fn, "",
                          PascalSErrno::ParseExpectSGotError), expected(expected), got(got) {
    this->msg = fmt::format("{} parse failed: "
                            "expected: {}, "
                            "got: {}", fn, expected, convertToString(got));
    if (!msg.empty()) {
        this->msg += ":" + msg;
    }
    if (got) {
        this->length = got->length;
        this->offset = got->offset;
        this->line = got->line;
        this->column = got->column;
    } else {
        this->length = 0;
        this->offset = 0;
        this->line = 1;
        this->column = 0;
    }
}

PascalSSemanticError::PascalSSemanticError(const char *fn, const pascal_s::Pos *problem_pos, std::string msg,
                                           PascalSErrno t) :
        PascalSError(std::move(msg), t), fn(fn) {
    if (problem_pos) {
        this->length = problem_pos->length;
        this->offset = problem_pos->offset;
        this->line = problem_pos->line;
        this->column = problem_pos->column;
    } else {
        this->length = 0;
        this->offset = 0;
        this->line = 1;
        this->column = 0;
    }
}
