
#include <string_view>
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
        if (all_lexed) {
            return nullptr;
        }

        lex_again:
        auto code = yylex();
        if (code == static_cast<lexer_action_code_underlying_type>(LexerActionCode::AuxFunctionCalled)) {
            goto lex_again;
        }
        if (code == static_cast<lexer_action_code_underlying_type>(LexerActionCode::LexEnd)) {
            if (current_token_cursor < tokens.size()) return tokens[current_token_cursor++];
            all_lexed = true;
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
    if (all_lexed) return tokens;

    while (yylex() != static_cast<lexer_action_code_underlying_type>(LexerActionCode::LexEnd));
    all_lexed = true;
    return tokens;
}

void FullInMemoryLexer::addError(ErrorToken *token) {
    errors.push_back(token);
}


int FullInMemoryLexer::addToken(Token *token) {
    token->line = yylineno;
    token->length = yyleng;
    token->column = current_offset - yyleng - line_offset;
    token->offset = current_offset - yyleng;
    tokens.push_back(token);
    return static_cast<lexer_action_code_underlying_type>(LexerActionCode::AppendToken);
}

const Lexer::error_references &FullInMemoryLexer::get_all_errors() {
    return errors;
}

bool FullInMemoryLexer::has_error() {
    return !errors.empty();
}

