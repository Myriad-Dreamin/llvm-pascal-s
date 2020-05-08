

#include <pascal-s/exception.h>
#include <pascal-s/token.h>
#include <pascal-s/ast.h>
#include <fmt/core.h>


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

PascalSParseExpectTGotError::PascalSParseExpectTGotError(
        char *fn, TokenType expected, const Token *got, std::string msg) :
        PascalSParseError(fn,
                fmt::format("runtime reinterpret node error:"
                            "expected type: {}, "
                            "got: {}", expected, got != nullptr ? got->type : TokenType::Nullptr)), expected(static_cast<token_type_underlying_type>(expected)), got(got) {
    if (!msg.empty()) {
        this-> msg += ":" + msg;
    }
}

PascalSParseExpectSGotError::PascalSParseExpectSGotError(
        char *fn, const char *expected, const Token *got, std::string msg) :
        PascalSParseError(fn,
                          fmt::format("runtime reinterpret node error:"
                                      "expected: {}, "
                                      "got: {}", expected, got != nullptr ? got->type : TokenType::Nullptr)), expected(expected), got(got) {
    if (!msg.empty()) {
        this-> msg += ":" + msg;
    }
}
