
#include <iostream>
#include <string_view>
#include <cstdarg>
#include <cstring>
#include <pascal-s/logger.h>
#include <pascal-s/token.h>
#include <pascal-s/lexer.h>

Lexer::Lexer(std::istream *in, std::ostream *out)
        : yyFlexLexer(in, out) {}


Lexer::~Lexer() {
}


int Lexer::addIdentifier() {
    return addToken(new Identifier(yytext));
}

int Lexer::addReal() {
    printf("found Real %s %d\n", yytext, xxx);
    warningf("todo!!!\n");
    return 1;
}

int Lexer::addInteger() {
    printf("found Integer %s %d\n", yytext, xxx);
    warningf("todo!!!\n");
    return 1;
}

int Lexer::addBoolean() {
    printf("found Boolean %s %d\n", yytext, xxx);
    warningf("todo!!!\n");
    return 1;
}

int Lexer::addKeyword() {
    printf("found Keyword %s %d\n", yytext, xxx);
    warningf("todo!!!\n");
    return 1;
}

int Lexer::addMarker() {
    printf("found Marker %s %d\n", yytext, xxx);
    warningf("todo!!!\n");
    return 1;
}

int Lexer::addChar() {
    printf("found Character %s %d\n", yytext, xxx);
    warningf("todo!!!\n");
    return 1;
}
