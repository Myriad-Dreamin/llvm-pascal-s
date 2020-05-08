//
// Created by kamiyoru on 2020/5/8.
//

#ifndef PASCAL_S_MOCK_H
#define PASCAL_S_MOCK_H

#include <vector>
#include <cassert>
#include "token.h"

struct MockLexer {
    const std::vector<Token*> &token_stream;

    explicit MockLexer(const std::vector<Token*> &token_stream) : token_stream(token_stream) {}

    // cursor控制peek_token的值

    using token_container = std::vector<Token *>;

    int current_token_cursor = 0;

    // 重置cursor
    [[maybe_unused]] void reset_cursor() {
        current_token_cursor = 0;}

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
};


#endif //PASCAL_S_MOCK_H
