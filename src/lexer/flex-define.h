
#ifndef PASCAL_S_FLEX_LEXER_DEFINITION
#define PASCAL_S_FLEX_LEXER_DEFINITION

#include <token.h>
#include <FlexLexer.h>
#include <iostream>
#include <string_view>
#include <cstdarg>
#include <logger.h>
#include <cstring>

struct Lexer : public yyFlexLexer {
    int xxx = 1;
    Logger logger;

    Lexer(std::istream *in = nullptr, std::ostream *out = nullptr)
        : yyFlexLexer(in, out) {}

public:

    int yylex() final;

    char *latest = nullptr;

    ~Lexer() override {
        clear_latest();
    }

private:
    void clear_latest() {
        if (latest) {
            delete[] latest;
            latest = nullptr;
        }
    }

    int addIdentifier() {
//        logger.warningf(_LMETA, "todo!!!\n");
        clear_latest();
        int l = strlen(yytext);
        latest = new char[l];
        strcpy(latest, yytext);
        return 1;
    }

    int addReal() {
        printf("found Real %s %d\n", yytext, xxx);
        warningf("todo!!!\n");
        return 1;
    }

    int addInteger() {
        printf("found Integer %s %d\n", yytext, xxx);
        warningf("todo!!!\n");
        return 1;
    }

    int addBoolean() {
        printf("found Boolean %s %d\n", yytext, xxx);
        warningf("todo!!!\n");
        return 1;
    }

    int addKeyword() {
        printf("found Keyword %s %d\n", yytext, xxx);
        warningf("todo!!!\n");
        return 1;
    }

    int addMarker() {
        printf("found Marker %s %d\n", yytext, xxx);
        warningf("todo!!!\n");
        return 1;
    }

    int addChar() {
        printf("found Character %s %d\n", yytext, xxx);
        warningf("todo!!!\n");
        return 1;
    }
};


#endif