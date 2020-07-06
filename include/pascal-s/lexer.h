//
// Created by kamiyoru on 2020/4/29.
//

#ifndef PASCAL_S_LEXER_H
#define PASCAL_S_LEXER_H

#include "lib/stdtype.h"
#include <FlexLexer.h>
#include "logger.h"
#include "token.h"

using lexer_action_code_underlying_type = int;
enum class LexerActionCode : lexer_action_code_underlying_type {
    LexEnd = 0,
    AppendToken = 1,
    AuxFunctionCalled = 2,
};

class Lexer : public yyFlexLexer {

public:
    enum LexerOptionEnum : uint16_t {
        LexerOptionLexComment = 1U << 1U,
    };

    uint16_t option_mask = 0;

    using token_container = std::vector<Token *>;
    using error_references = std::vector<ErrorToken *>;

    explicit Lexer(std::istream *in = nullptr, std::ostream *out = nullptr);

    ~Lexer() override;

    void setOption(uint16_t om) { this->option_mask = om; }

    virtual void reset_cursor() = 0;

    virtual const Token *next_token() = 0;

    virtual const Token *peek_token() = 0;

    virtual const token_container &get_all_tokens() = 0;

    virtual const error_references &get_all_errors() = 0;

    virtual bool has_error() = 0;

protected:
    int yylex() final;

    Logger logger;

    // current_offset由gulp.h中的YY_USER_ACTION更新
    pascal_s::column_t current_offset = 0, line_offset = 0;

    virtual int addToken(Token *token) = 0;

    virtual void addError(ErrorToken *token) = 0;

private:
    int comment_embed = 0;

    int addIdentifier();

    int addReal();

    int addInteger();

    int addBoolean();

    int addKeyword();

    int addMarker();

    int addComment();

    int addInlineComment();

    int addCommentAux();

    int addASCIIChar();

    int addChar();

    int recordNewLine();

    int skipErrorString(const char *hint = "occurs a lexical error here");
};

class FullInMemoryLexer : public Lexer {
    token_container tokens;
    error_references errors;
    int64_t current_token_cursor;
    bool all_lexed = false;
public:
    explicit FullInMemoryLexer(std::istream *in = nullptr, std::ostream *out = nullptr);

    ~FullInMemoryLexer() override;

    void reset_cursor() override;

    const Token *next_token() override;

    const Token *peek_token() override;

    const token_container &get_all_tokens() override;

    const error_references &get_all_errors() override;

    bool has_error() override;

private:
    int addToken(Token *token) override;

    void addError(ErrorToken *token) override;
};

#endif //PASCAL_S_LEXER_H
