
#include <pascal-s/token.h>
#include <cstring>
#include <fmt/core.h>
#include <cassert>
#include <cstdlib>

void deleteToken(Token *pToken) {
    switch (pToken->type) {
        case TokenType::Keyword:
            delete reinterpret_cast<Keyword *>(pToken);
            break;
        case TokenType::Marker:
            delete reinterpret_cast<Marker *>(pToken);
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

std::string convertToString(const Token *pToken) {
    switch (pToken->type) {
        case TokenType::Keyword:
            return fmt::format("{{ .type = Keyword .key_type = {} }}",
                               get_keyword_type_reversed(reinterpret_cast<const Keyword *>(pToken)->key_type));
        case TokenType::Marker:
            return fmt::format("{{ .type = Marker .marker_type = {} }}",
                               reinterpret_cast<const Marker *>(pToken)->content);
        case TokenType::ConstantString:
            return fmt::format("{{ .type = ConstantString .content = {} }}",
                               reinterpret_cast<const ConstantString *>(pToken)->content);
        case TokenType::ConstantReal:
            return fmt::format("{{ .type = ConstantReal .content = {} }}",
                               reinterpret_cast<const ConstantReal *>(pToken)->content);
        case TokenType::ConstantInteger:
            return fmt::format("{{ .type = ConstantInteger .content = {} }}",
                               reinterpret_cast<const ConstantInteger *>(pToken)->content);
        case TokenType::ConstantChar:
            return fmt::format("{{ .type = ConstantChar .content = {} }}",
                               reinterpret_cast<const ConstantChar *>(pToken)->content);
        case TokenType::Identifier:
            return fmt::format("{{ .type = Identifier .content = {} }}",
                               reinterpret_cast<const Identifier *>(pToken)->content);
        default:
            throw RuntimeReinterpretTokenException(pToken);
    }
    assert(false);
}

ConstantReal::ConstantReal(const char *creal) : Token() {
    this->type = TokenType::ConstantReal;
    int l = strlen(creal);
    this->content = new char[l + 1];
    strcpy(const_cast<char *>(content), creal);
    //bug: not safe
    this->attr = strtod(content, NULL);
}

ConstantReal::~ConstantReal() {
    delete[]content;
}

ConstantInteger::ConstantInteger(const char *cint) : Token() {
    this->type = TokenType::ConstantInteger;
    int l = strlen(cint);
    this->content = new char[l + 1];
    strcpy(const_cast<char *>(content), cint);
    //bug: not safe
    this->attr = strtoll(content, NULL, 10);
}

ConstantInteger::~ConstantInteger() {
    delete[]content;
}

ConstantChar::ConstantChar(const char *cchar) : Token() {
    this->type = TokenType::ConstantChar;
    int l = strlen(cchar);
    this->content = new char[l + 1];
    strcpy(const_cast<char *>(content), cchar);
    this->attr = content;
}

ConstantChar::~ConstantChar() {
    delete[]content;
}

Identifier::Identifier(const char *identifier) : Token() {
    this->type = TokenType::Identifier;
    int l = strlen(identifier);
    this->content = new char[l + 1];
    strcpy(const_cast<char *>(content), identifier);
}

Identifier::~Identifier() {
    delete[]content;
}

ConstantBoolean::ConstantBoolean(const char *cbool) : Token() {
    this->type = TokenType::ConstantBoolean;
    int l = strlen(cbool);
    this->content = new char[l + 1];
    strcpy(const_cast<char *>(content), cbool);
    if (this->content[0] == 't')
        this->attr = true;
    else if (this->content[0] == 'f')
        this->attr = false;
    else
        assert(false);
}

ConstantBoolean::~ConstantBoolean() {
    delete[]content;
}

Marker::Marker(const char *cmarker) : Token() {
    this->type = TokenType::Marker;
    int l = strlen(cmarker);
    content = new char[l + 1];
    strcpy(const_cast<char *>(content), cmarker);
}

Marker::~Marker() {
    delete[]content;
}

keyword_mapping key_map = {
        keyword_mapping::value_type{"to", KeywordType::To},
        keyword_mapping::value_type{"do", KeywordType::Do},
        keyword_mapping::value_type{"of", KeywordType::Of},
        keyword_mapping::value_type{"for", KeywordType::For},
        keyword_mapping::value_type{"else", KeywordType::Else},
        keyword_mapping::value_type{"if", KeywordType::If},
        keyword_mapping::value_type{"then", KeywordType::Then},
        keyword_mapping::value_type{"char", KeywordType::Char},
        keyword_mapping::value_type{"boolean", KeywordType::Boolean},
        keyword_mapping::value_type{"real", KeywordType::Real},
        keyword_mapping::value_type{"integer", KeywordType::Integer},
        keyword_mapping::value_type{"array", KeywordType::Array},
        keyword_mapping::value_type{"end", KeywordType::End},
        keyword_mapping::value_type{"begin", KeywordType::Begin},
        keyword_mapping::value_type{"function", KeywordType::Function},
        keyword_mapping::value_type{"procedure", KeywordType::Procedure},
        keyword_mapping::value_type{"program", KeywordType::Program},
        keyword_mapping::value_type{"var", KeywordType::Var},
        keyword_mapping::value_type{"const", KeywordType::Const}
};

reverse_keyword_mapping reverse_key_map;

const char *get_keyword_type_reversed(KeywordType kt) {
    if (reverse_key_map.empty()) {
        for (auto &kv: key_map) {
            reverse_key_map[kv.second] = kv.first.c_str();
        }
    }
    return reverse_key_map.at(kt);
}