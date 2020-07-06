//
// Created by kamiyoru on 2020/7/3.
//

#ifndef PASCAL_S_LEXER_MOCK_H
#define PASCAL_S_LEXER_MOCK_H


#include <vector>
#include <cassert>
#include "token.h"

struct MockLexer {
    const std::vector<Token *> &token_stream;
    const std::vector<ErrorToken *> nil_errors;

    explicit MockLexer(const std::vector<Token *> &token_stream) : token_stream(token_stream), nil_errors() {}

    // cursor控制peek_token的值

    using token_container = std::vector<Token *>;
    using error_references = std::vector<ErrorToken *>;

    int current_token_cursor = 0;

    // 重置cursor
    [[maybe_unused]] void reset_cursor() {
        current_token_cursor = 0;
    }

    // 返回当前token的值并移动cursor
    [[maybe_unused]] const Token *next_token() {
        return current_token_cursor < token_stream.size() ? token_stream[current_token_cursor++] : nullptr;
    }

    // 返回当前token的值且不移动cursor
    [[maybe_unused]] const Token *peek_token() {

        assert(current_token_cursor <= token_stream.size());
        if (current_token_cursor == 0) {
            return next_token();
        }
        return token_stream[current_token_cursor - 1];
    }

    // 获取所有的token
    [[maybe_unused]] const token_container &get_all_tokens() {
        return token_stream;
    }

    // 获取所有的词法错误
    [[maybe_unused]] const error_references &get_all_errors() { return nil_errors; }

    // 判断是否存在词法错误
    [[maybe_unused]] bool has_error() { return false; }
};


#endif //PASCAL_S_LEXER_MOCK_H
