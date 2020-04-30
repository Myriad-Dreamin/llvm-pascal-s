//
// Created by kamiyoru on 2020/4/29.
//

#ifndef PASCAL_S_LEXER_H
#define PASCAL_S_LEXER_H

#include <FlexLexer.h>
#include "logger.h"
#include "token.h"



class Lexer : public yyFlexLexer {
public:
    using token_container = std::vector<Token *>;

    explicit Lexer(std::istream *in = nullptr, std::ostream *out = nullptr);

    ~Lexer() override;

    virtual const Token *next_token() = 0;

    virtual const token_container &get_all_tokens() = 0;

protected:
    int yylex() final;
    Logger logger;
    column_t current_offset = 0, line_offset = 0;

    virtual int addToken(Token *token) = 0;

private:

    int addIdentifier();

    int addReal();

    int addInteger();

    int addBoolean();

    int addKeyword();

    int addMarker();

    int addChar();

    int recordNewLine();
};

class FullInMemoryLexer : public Lexer {
    token_container tokens;
    int64_t current_token_cursor;
public:
    explicit FullInMemoryLexer(std::istream *in = nullptr, std::ostream *out = nullptr);

    ~FullInMemoryLexer() override;

    void reset_cursor();

    const Token *next_token() final;

    const token_container &get_all_tokens() final;

private:
    int addToken(Token *token) final;
};

#endif //PASCAL_S_LEXER_H
