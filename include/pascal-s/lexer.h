//
// Created by kamiyoru on 2020/4/29.
//

#ifndef PASCAL_S_LEXER_H
#define PASCAL_S_LEXER_H

#include <FlexLexer.h>
#include "logger.h"
#include "token.h"

struct Lexer : public yyFlexLexer {
    using token_container = std::vector<Token *>;

    int xxx = 1;
    Logger logger;
    token_container tokens;

    Lexer(std::istream *in = nullptr, std::ostream *out = nullptr);

    // todo: remove
    char *latest = nullptr;

public:

    int yylex() final;

    ~Lexer() override;

    const Token *next_token();

    const token_container &get_all_tokens();

private:

    void clear_latest();

    int addToken(Token *token);

    int addIdentifier();

    int addReal();

    int addInteger();

    int addBoolean();

    int addKeyword();

    int addMarker();

    int addChar();
};

#endif //PASCAL_S_LEXER_H
