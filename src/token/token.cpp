
#include <pascal-s/token.h>

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
        default:
            throw RuntimeReinterpretTokenException(pToken);
    }
}