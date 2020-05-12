
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
                               get_marker_type_reversed(reinterpret_cast<const Marker *>(pToken)->marker_type));
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
                               reinterpret_cast<const ConstantChar *>(pToken)->value);
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

namespace char_spec {
    static struct _spec_char_map_t {
        char inner[128]{};

        explicit _spec_char_map_t() noexcept {
            inner['n'] = '\n';
            inner['t'] = '\t';
            inner['r'] = '\r';
            inner['\\'] = '\\';
//            inner['n'] = '\n';
        }

        char operator[](char a) {
            assert(inner[a] != 0);
            return inner[a];
        }
    } spec_char_map;
}

ConstantChar::ConstantChar(const char *cchar) : Token() {
    this->type = TokenType::ConstantChar;
    assert(cchar[0] == '\'');
    if (cchar[1] == '\00') {
        assert(false);
    } else if (cchar[2] == '\'') {
        this->value = cchar[1];
    } else {
        assert(cchar[4] == '\00');
        assert(cchar[1] == '\'');
        this->value = char_spec::spec_char_map[cchar[2]];
    }
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

Keyword::Keyword(KeywordType key_type) noexcept: Token(), key_type(key_type) {
    this->type = TokenType::Keyword;
}


Marker::Marker(MarkerType marker_type) noexcept: Token(), marker_type(marker_type) {
    this->type = TokenType::Marker;
}

Marker::~Marker() = default;

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

marker_mapping marker_map = {
        marker_mapping::value_type{"<>", MarkerType::NEQ},
        marker_mapping::value_type{"<=", MarkerType::LE},
        marker_mapping::value_type{">=", MarkerType::GE},
        marker_mapping::value_type{"<", MarkerType::LT},
        marker_mapping::value_type{"=", MarkerType::EQ},
        marker_mapping::value_type{">", MarkerType::GT},
        marker_mapping::value_type{"..", MarkerType::Range},

        marker_mapping::value_type{":=", MarkerType::Assign},
        marker_mapping::value_type{"+", MarkerType::Add},
        marker_mapping::value_type{"-", MarkerType::Sub},
        marker_mapping::value_type{"*", MarkerType::Mul},
        marker_mapping::value_type{"/", MarkerType::Div},
        marker_mapping::value_type{"mod", MarkerType::Mod},

        marker_mapping::value_type{"(", MarkerType::LParen},
        marker_mapping::value_type{")", MarkerType::RParen},
        marker_mapping::value_type{"[", MarkerType::LBracket},
        marker_mapping::value_type{"]", MarkerType::RBracket},

        marker_mapping::value_type{",", MarkerType::Comma},
        marker_mapping::value_type{".", MarkerType::Dot},
        marker_mapping::value_type{";", MarkerType::Semicolon},
        marker_mapping::value_type{":", MarkerType::Colon},
};

reverse_marker_mapping reverse_marker_map;

const char *get_marker_type_reversed(MarkerType kt) {
    if (reverse_marker_map.empty()) {
        for (auto &kv: marker_map) {
            reverse_marker_map[kv.second] = kv.first.c_str();
        }
    }
    return reverse_marker_map.at(kt);
}

marker_type_underlying_type get_marker_pri(MarkerType marker_type) {
    return static_cast<marker_type_underlying_type>(marker_type) >> 0x4U;
}


namespace predicate {
#define marker_predicator(lower, upper) bool is_ ## lower(const Token *tok) {\
    return tok != nullptr && tok->type == TokenType::Marker && reinterpret_cast<const Marker*>(tok)-> marker_type == MarkerType::upper; \
}\
const Marker marker_## lower(MarkerType::upper);
#define keyword_predicator(lower, upper) bool is_ ## lower(const Token *tok) {\
    return tok != nullptr && tok->type == TokenType::Keyword && reinterpret_cast<const Keyword*>(tok)-> key_type == KeywordType::upper; \
}\
const Keyword keyword_## lower(KeywordType::upper);

    marker_predicator(neq, NEQ)
    marker_predicator(le, LE)
    marker_predicator(ge, GE)
    marker_predicator(lt, LT)
    marker_predicator(eq, EQ)
    marker_predicator(gt, GT)
    marker_predicator(range, Range)

    marker_predicator(assgin, Assign)
    marker_predicator(add, Add)
    marker_predicator(sub, Sub)
    marker_predicator(mul, Mul)
    marker_predicator(div, Div)

    marker_predicator(mod, Mod)

    marker_predicator(lparen, LParen)
    marker_predicator(rparen, RParen)
    marker_predicator(lbracket, LBracket)
    marker_predicator(rbracket, RBracket)

    marker_predicator(comma, Comma)
    marker_predicator(dot, Dot)
    marker_predicator(semicolon, Semicolon)
    marker_predicator(colon, Colon)

    keyword_predicator(program, Program)
    keyword_predicator(const, Const)
    keyword_predicator(var, Var)
    keyword_predicator(procedure, Procedure)
    keyword_predicator(function, Function)
    keyword_predicator(begin, Begin)
    keyword_predicator(end, End)

    keyword_predicator(array, Array)
    keyword_predicator(integer, Integer)
    keyword_predicator(real, Real)
    keyword_predicator(boolean, Boolean)
    keyword_predicator(char, Char)

    keyword_predicator(if, If)
    keyword_predicator(then, Then)
    keyword_predicator(else, Else)
    keyword_predicator(for, For)
    keyword_predicator(to, To)
    keyword_predicator(do, Do)
    keyword_predicator(of, Of)

    bool token_equal(const Token *lhs, const std::vector<Token *> *rhs) {
        if (rhs == nullptr) {
            return lhs == nullptr;
        }

        for (auto r: *rhs) {
            if (token_equal(lhs, r)) {
                return true;
            }
        }
        return false;
    }

    bool token_equal(const Token *lhs, const std::set<const Token *> *rhs) {
        if (rhs == nullptr) {
            return lhs == nullptr;
        }

        for (auto r: *rhs) {
            if (token_equal(lhs, r)) {
                return true;
            }
        }
        return false;
    }

    bool token_equal(const Token *lhs, const Token *rhs) {
        if (lhs == nullptr && rhs == nullptr) {
            return true;
        }
        if (lhs == nullptr || rhs == nullptr) {
            return false;
        }
        if (lhs->type != rhs->type) {
            return false;
        }
        switch (lhs->type) {
            case TokenType::Marker:
                return reinterpret_cast<const Marker *>(lhs)->marker_type ==
                       reinterpret_cast<const Marker *>(rhs)->marker_type;
            case TokenType::Keyword:
                return reinterpret_cast<const Keyword *>(lhs)->key_type ==
                       reinterpret_cast<const Keyword *>(rhs)->key_type;
            default:
                throw std::runtime_error("todo token equal");
        }
    }

#undef marker_predicator
#undef keyword_predicator
}