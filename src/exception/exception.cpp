

#include <pascal-s/exception.h>
#include <pascal-s/token.h>
#include <fmt/core.h>


RuntimeReinterpretTokenException::RuntimeReinterpretTokenException(const Token *token)
        : token(token), msg(fmt::format("runtime reinterpret token error: unknown type: {}", token->type)) {
}

const char *RuntimeReinterpretTokenException::what() const _GLIBCXX_USE_NOEXCEPT {
//
    return msg.c_str();
}