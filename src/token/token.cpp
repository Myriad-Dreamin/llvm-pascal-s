
#include <pascal-s/token.h>
#include <cstring>

void deleteToken(Token *pToken) {
    switch (pToken->type) {
        case TokenType::Keyword:
            delete reinterpret_cast<Keyword *>(pToken);
            break;
        case TokenType::ConstantString:
            delete reinterpret_cast<ConstantString *>(pToken);
            break;
        case TokenType::ConstantReal:
            delete reinterpret_cast<ConstantReal *>(pToken);
            break;
        case TokenType::ConstantInteger:
            delete reinterpret_cast<ConstantInteger *>(pToken);
            break;
        case TokenType::ConstantChar:
            delete reinterpret_cast<ConstantChar *>(pToken);
            break;
        case TokenType::Identifier:
            delete reinterpret_cast<Identifier *>(pToken);
            break;
        default:
            throw RuntimeReinterpretTokenException(pToken);
    }
}

Identifier::Identifier(const char *identifier) : Token() {
    this->type = TokenType::Identifier;
    int l = strlen(identifier);
    content = new char[l + 1];
    strcpy(const_cast<char *>(content), identifier);
}

Identifier::~Identifier() {
    delete[]content;
}