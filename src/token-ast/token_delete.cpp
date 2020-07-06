

#include <pascal-s/token.h>


void deleteToken(Token *pToken) {

    if (predicate::is_marker_type(pToken->type)) {
        delete reinterpret_cast<Marker *>(pToken);
        return;
    }

    if (predicate::is_keyword_type(pToken->type)) {
        delete reinterpret_cast<Marker *>(pToken);
        return;
    }

    switch (pToken->type) {
        case TokenType::ConstantString:
        case TokenType::ConstantReal:
            delete reinterpret_cast<ConstantReal *>(pToken);
            break;
        case TokenType::ConstantInteger:
            delete reinterpret_cast<ConstantInteger *>(pToken);
            break;
        case TokenType::ConstantChar:
            delete reinterpret_cast<ConstantChar *>(pToken);
            break;
        case TokenType::ConstantBoolean:
            delete reinterpret_cast<ConstantBoolean *>(pToken);
            break;
        case TokenType::Identifier:
            delete reinterpret_cast<Identifier *>(pToken);
            break;
        case TokenType::ErrorToken:
            delete reinterpret_cast<ErrorToken *>(pToken);
            break;
        case TokenType::Comment:
            delete reinterpret_cast<Comment *>(pToken);
            break;
        default:
            throw RuntimeReinterpretTokenException(pToken);
    }
}
