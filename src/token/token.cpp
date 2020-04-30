
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

ConstantReal::ConstantReal(const char *creal) : Token() {
    this->type = TokenType::ConstantReal;
    int l = strlen(creal);
    content = new char[l + 1];
    strcpy(const_cast<char *>(content), creal);
}

ConstantReal::~ConstantReal() {
    delete[]content;
}

ConstantInteger::ConstantInteger(const char *cint) : Token() {
    this->type = TokenType::ConstantInteger;
    int l = strlen(cint);
    this->content = new char[l + 1];
    strcpy(const_cast<char *>(content), cint);
}

ConstantInteger::~ConstantInteger() {
    delete[]content;
}

ConstantChar::ConstantChar(const char *cchar) : Token() {
    this->type = TokenType::ConstantChar;
    int l = strlen(cchar);
    this->content = new char[l + 1];
    strcpy(const_cast<char *>(content), cchar);

}

ConstantChar::~ConstantChar() {
    delete[]content;
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

ConstantBoolean::ConstantBoolean(const char *cbool) : Token() {
    this->type = TokenType::ConstantBoolean;
    int l = strlen(cbool);
    content = new char[l + 1];
    strcpy(const_cast<char *>(content), cbool);
}

ConstantBoolean::~ConstantBoolean() {
    delete[]content;
}

Marker::Marker(const char *cmarker) : Token() {
    this->type = TokenType::Marker;
    int l = strlen(cmarker);
    content = new char[l+1];
    strcpy(const_cast<char *>(content), cmarker);
}

Marker::~Marker() {
    delete[]content;
}
