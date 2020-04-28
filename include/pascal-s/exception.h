
#ifndef PASCALS_STD_EXCEPTION
#define PASCALS_STD_EXCEPTION

#include <exception>
#include <string>

struct Token;

class RuntimeReinterpretTokenException : public std::exception {
    const Token *token;
    std::string msg;
public:
    explicit RuntimeReinterpretTokenException(const Token *token);

    [[nodiscard]] const char *what() const _GLIBCXX_USE_NOEXCEPT override;
};

#endif

