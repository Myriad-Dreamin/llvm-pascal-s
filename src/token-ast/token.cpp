
#include <fmt/core.h>
#include <pascal-s/token.h>
#include <string>


char *copy_string(const char *content, int length) {
    char *ns = new char[length + 1];
    strcpy(ns, content);
    return ns;
}

char *copy_string(const char *content) {
    return copy_string(content, strlen(content));
}

const char *convertToString(TokenType tt) {
    if (predicate::is_marker_type(tt)) {
        return "TokenType::Marker";
    }

    if (predicate::is_keyword_type(tt)) {
        return "TokenType::Keyword";
    }

    switch (tt) {
        case TokenType::Unknown:
            return "TokenType::Unknown";
        case TokenType::ConstantString:
            return "TokenType::ConstantString";
        case TokenType::ConstantChar:
            return "TokenType::ConstantChar";
        case TokenType::ConstantReal:
            return "TokenType::ConstantReal";
        case TokenType::ConstantInteger:
            return "TokenType::ConstantInteger";
        case TokenType::ConstantBoolean:
            return "TokenType::ConstantBoolean";
        case TokenType::Identifier:
            return "TokenType::Identifier";
        case TokenType::Nullptr:
            return "TokenType::Nullptr";
        case TokenType::ErrorToken:
            return "TokenType::ErrorToken";
        case TokenType::Length:
            return "TokenType::Length";
        default:
            assert(false);
    }
}

std::string convertToString(const Token *pToken) {
    if (pToken == nullptr) {
        return "nullptr";
    }

    if (predicate::is_marker_type(pToken->type)) {

        return fmt::format(
                "{{ .type = Marker .marker_type = {} }}",
                get_marker_type_reversed(
                        reinterpret_cast<const Marker *>(pToken)->type));
    }

    if (predicate::is_keyword_type(pToken->type)) {
        return fmt::format("{{ .type = Keyword .key_type = {} }}",
                           get_keyword_type_reversed(
                                   reinterpret_cast<const Keyword *>(pToken)->type));
    }

    switch (pToken->type) {
        case TokenType::ConstantString:
            return fmt::format(
                    "{{ .type = ConstantString .attr = {} }}",
                    reinterpret_cast<const ConstantString *>(pToken)->attr);
        case TokenType::ConstantReal:
            return fmt::format(
                    "{{ .type = ConstantReal .attr = {}, .raw = {} }}",
                    reinterpret_cast<const ConstantReal *>(pToken)->attr,
                    reinterpret_cast<const ConstantReal *>(pToken)->content);
        case TokenType::ConstantInteger:
            return fmt::format(
                    "{{ .type = ConstantInteger .attr = {} }}",
                    reinterpret_cast<const ConstantInteger *>(pToken)->attr);
        case TokenType::ConstantChar:
            return fmt::format(
                    "{{ .type = ConstantChar .attr = {} }}",
                    reinterpret_cast<const ConstantChar *>(pToken)->attr);
        case TokenType::ConstantBoolean:
            return fmt::format(
                    "{{ .type = ConstantBoolean .attr = {} }}",
                    reinterpret_cast<const ConstantBoolean *>(pToken)->attr);
        case TokenType::Identifier:
            return fmt::format(
                    "{{ .type = Identifier .content = {} }}",
                    reinterpret_cast<const Identifier *>(pToken)->content);
        case TokenType::ErrorToken:
            return fmt::format(
                    "{{ .type = LexerError .content = {} }}",
                    reinterpret_cast<const ErrorToken *>(pToken)->content);
        default:
            return fmt::format(
                    "{{ .type = LexerError .content = {} }}",
                    "default type");
            //throw RuntimeReinterpretTokenException(pToken);
    }
    assert(false);
}


ErrorToken::ErrorToken(const char *content, const char *hint)
        : Token() {
    this->type = TokenType::ErrorToken;
    this->content = copy_string(content);
    this->hint = hint;
}

ErrorToken::ErrorToken(const char *content, int len, const char *hint)
        : Token() {
    this->type = TokenType::ErrorToken;
    this->length = len;
    this->content = copy_string(content);
    this->hint = hint;
}

ErrorToken::~ErrorToken() {
    delete[] this->content;
    this->content = nullptr;
    delete[] this->hint;
    this->hint = nullptr;
}

ErrorToken *ErrorToken::copy_in(const char *content, const char *hint) {
    return new ErrorToken(content, copy_string(hint));
}

ConstantReal::ConstantReal(const char *content, double attr) : Token() {
    this->type = TokenType::ConstantReal;
    this->content = copy_string(content);
    this->attr = attr;
}

ConstantReal::~ConstantReal() {
    delete[] this->content;
    this->content = nullptr;
}

ConstantInteger::ConstantInteger(pascal_s::pascal_s_integer_t attr) : Token() {
    this->type = TokenType::ConstantInteger;
    this->attr = attr;
}

ConstantInteger::~ConstantInteger() = default;

ConstantChar::ConstantChar(char ch) : Token() {
    this->type = TokenType::ConstantChar;
    this->attr = ch;
}

ConstantChar::~ConstantChar() = default;

Identifier::Identifier(const char *content) : Token() {
    this->type = TokenType::Identifier;
    this->content = copy_string(content);
}

Identifier::~Identifier() {
    delete[]content;
}

ConstantBoolean::ConstantBoolean(bool attr) : Token() {
    this->type = TokenType::ConstantBoolean;
    this->attr = attr;
}

ConstantBoolean::~ConstantBoolean() = default;


Comment::Comment(const char *content) {
    this->type = TokenType::Comment;
    this->content = copy_string(content);
}
