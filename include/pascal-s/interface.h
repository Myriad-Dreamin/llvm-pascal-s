//
// Created by kamiyoru on 2020/5/5.
//

#ifndef PASCAL_S_INTERFACE_H
#define PASCAL_S_INTERFACE_H

#include "token.h"
#include "ast.h"
#include <vector>

// reserve lvalue reference semantics
// ProxyTarget
// 1. rvalue reference: ProxyName& = ProxyTarget&& & = ProxyTarget&
// 1. lvalue reference: ProxyName& = ProxyTarget& & = ProxyTarget&
// 1. origin value: ProxyName& = ProxyTarget & = ProxyTarget&
#define DefaultProxyConstructor(ClassName, ProxyName, proxy_field)\
    ProxyName &proxy_field;\
    explicit ClassName(ProxyName &proxy_field) : proxy_field(proxy_field) {}\
    ClassName(const ClassName &p) : proxy_field(p.proxy_field) {}\
    ClassName(ClassName &&p) noexcept : proxy_field(p.proxy_field) {}\
    ClassName &operator=(const ClassName &p) noexcept {\
        proxy_field = p.proxy_field;\
        return *this; }\
    ClassName &operator=(ClassName &&p)  noexcept {\
        proxy_field = p.proxy_field;\
        return *this; }

// auto generated LexerProxy struct
// you can partially specialize a specified type to change proxy behavior
template<typename Lexer>
struct LexerProxy {
    DefaultProxyConstructor(LexerProxy, Lexer, lexer)

    using token_container = std::vector<Token *>;

    // cursor控制peek_token的值

    // 重置cursor
    [[maybe_unused]] void reset_cursor() { lexer.reset_cursor(); }

    // 返回当前token的值并移动cursor
    [[maybe_unused]] const Token *next_token() { return lexer.next_token(); }

    // 返回当前token的值且不移动cursor
    [[maybe_unused]] const Token *peek_token() { return lexer.peek_token(); }

    // 获取所有的token
    [[maybe_unused]] const token_container &get_all_tokens() { return lexer.get_all_tokens(); }
};

// auto generated OStreamProxy struct
// you can partially specialize a specified type to change proxy behavior
template<typename OStream>
struct OStreamProxy {
    DefaultProxyConstructor(OStreamProxy, OStream, os)

    // 定义一个OStream 为 operator<<(T data)

    // T至少重载一个const char*，其他的可以看心情重载

    // 重置cursor
    template<typename T>
    [[maybe_unused]] OStreamProxy &operator<<(T data) {
        os << data;
        return *this;
    }

    [[maybe_unused]] OStreamProxy &write_data(const char *data) {
        return operator<<(data);
    }

    [[maybe_unused]] OStreamProxy &write_data(const std::string &data) {
        return operator<<(data.c_str());
    }

    template<typename T>
    [[maybe_unused]] OStreamProxy &write_data(const T &data) {
        return operator<<(data.c_str());
    }
};

template<typename Parser>
struct ParserProxy {
    DefaultProxyConstructor(ParserProxy, Parser, parser)

    // cursor控制peek_token的值

    // 重置cursor
    [[maybe_unused]] ast::Node *parse() { return parser.parse(); }
};

#endif //PASCAL_S_INTERFACE_H
