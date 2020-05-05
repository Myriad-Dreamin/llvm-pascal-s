//
// Created by kamiyoru on 2020/5/5.
//

#ifndef PASCAL_S_INTERFACE_H
#define PASCAL_S_INTERFACE_H

#include "token.h"
#include <vector>

#define DefaultProxyConstructor(ClassName, ProxyName, ProxyField)\
    ProxyName &ProxyField;\
    explicit ClassName(ProxyName &ProxyField) : ProxyField(ProxyField) {}\
    ClassName(const ClassName &p) : ProxyField(p.ProxyField) {}\
    ClassName(ClassName &&p) noexcept : ProxyField(p.ProxyField) {}\
    ClassName &operator=(const ClassName &p) noexcept {\
        ProxyField = p.ProxyField;\
        return *this; }\
    ClassName &operator=(ClassName &&p)  noexcept {\
    ProxyField = p.ProxyField;\
    return *this; }

// auto generated LexerProxy struct
// you can partially specialize a specified type to change Proxy Behavior
template<typename Lexer>
struct LexerProxy {
    DefaultProxyConstructor(LexerProxy, Lexer, lexer)

    using token_container = std::vector<Token *>;

    [[maybe_unused]] void reset_cursor() { lexer.reset_cursor(); }

    [[maybe_unused]] const Token *next_token() { return lexer.next_token(); }

    [[maybe_unused]] const Token *peek_token() { return lexer.peek_token(); }

    [[maybe_unused]] const token_container &get_all_tokens() { return lexer.get_all_tokens(); }
};

// auto generated LexerProxy struct
// you can partially specialize a specified type to change Proxy Behavior
template<typename OStream>
struct OStreamProxy {
    DefaultProxyConstructor(OStreamProxy, OStream, os)

    template<typename T>
    [[maybe_unused]] OStreamProxy &operator<<(T v) {
        os << v;
        return *this;
    }
};


#endif //PASCAL_S_INTERFACE_H
