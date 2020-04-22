
#ifndef PASCAL_S_FLEX_LEXER_DEFINITION
#define PASCAL_S_FLEX_LEXER_DEFINITION

#include <token.h>
#include <FlexLexer.h>
#include <iostream>
#include <string_view>
#include <cstdarg>
#include <logger.h>

struct Lexer : public yyFlexLexer {
    int xxx=1;
    Logger logger;

public:

    int yylex();

private:
    int addIdentifier() {
        printf("found identifier %s %d\n", yytext, xxx);
        logger.warningf(__FILE__ " %s:%d: todo!!!\n", __FUNCTION__, __LINE__);
        return 1;
    }

    int addReal() {
        printf("found Real %s %d\n", yytext, xxx);
        logger.warningf(__FILE__ " %s:%d: todo!!!\n", __FUNCTION__, __LINE__);
        return 1;
    }

    int addInteger() {
        printf("found Integer %s %d\n", yytext, xxx);
        logger.warningf(__FILE__ " %s:%d: todo!!!\n", __FUNCTION__, __LINE__);
        return 1;
    }

    int addBoolean() {
        printf("found Boolean %s %d\n", yytext, xxx);
        logger.warningf(__FILE__ " %s:%d: todo!!!\n", __FUNCTION__, __LINE__);
        return 1;
    }

    int addKeyword() {
        printf("found Keyword %s %d\n", yytext, xxx);
        logger.warningf(__FILE__ " %s:%d: todo!!!\n", __FUNCTION__, __LINE__);
        return 1;
    }

    int addMarker() {
        printf("found Marker %s %d\n", yytext, xxx);
        logger.warningf(__FILE__ " %s:%d: todo!!!\n", __FUNCTION__, __LINE__);
        return 1;
    }

    int addChar() {
        printf("found Character %s %d\n", yytext, xxx);
        logger.warningf(__FILE__ " %s:%d: todo!!!\n", __FUNCTION__, __LINE__);
        return 1;
    }
};


#endif