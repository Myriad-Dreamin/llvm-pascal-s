
#include <iostream>
#include <string_view>
#include <cstdarg>
#include <cstring>
#include <pascal-s/logger.h>
#include <pascal-s/token.h>
#include <pascal-s/lexer.h>
#include <cassert>

//FullInMemoryLexer::FullInMemoryLexer(std::istream *in, std::ostream *out)
//        : yyFlexFullInMemoryLexer(in, out) {}


FullInMemoryLexer::FullInMemoryLexer(std::istream *in, std::ostream *out)
        : Lexer(in, out) {
    current_token_cursor = 0;
}

FullInMemoryLexer::~FullInMemoryLexer() {
    for (auto &token :tokens) {
        deleteToken(token);
    }
}

void FullInMemoryLexer::reset_cursor() {
    current_token_cursor = 0;
}

const Token *FullInMemoryLexer::next_token() {
    if (current_token_cursor >= tokens.size()) {
        auto code = yylex();
        if (code == 1000) {
            return next_token();
        }
        if (code == 0) {
            return nullptr;
        }
    }
    assert(current_token_cursor < tokens.size());
    return tokens[current_token_cursor++];
}

const Token *FullInMemoryLexer::peek_token() {
    assert(current_token_cursor <= tokens.size());
    if (current_token_cursor == 0) {
        return next_token();
    }
    return tokens[current_token_cursor - 1];
}

const FullInMemoryLexer::token_container &FullInMemoryLexer::get_all_tokens() {
    while (yylex() != 0);
    return tokens;
}

int FullInMemoryLexer::addToken(Token *token) {
    token->line = yylineno;
    token->column = current_offset - line_offset - yyleng;
    tokens.push_back(token);
    return 1;
}

