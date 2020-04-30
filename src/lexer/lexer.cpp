
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
    printf("found identifier %s %d \t in line %d\n", yytext, xxx, numOfLine);
    Identifier *tok = new Identifier(yytext);
    tok->line = numOfLine;
    return addToken(tok);
}

int Lexer::addReal() {
    printf("found Real %s %d \t in line %d\n", yytext, xxx, numOfLine);
    ConstantReal *tok = new ConstantReal(yytext);
    tok->line = numOfLine;
    return addToken(tok);
}

int Lexer::addInteger() {
    printf("found Integer %s %d \t in line %d\n", yytext, xxx, numOfLine);
    ConstantInteger *tok = new ConstantInteger(yytext);
    tok->line = numOfLine;
    return addToken(tok);
}

int Lexer::addBoolean() {
    printf("found Boolean %s %d \t in line %d\n", yytext, xxx, numOfLine);
    ConstantBoolean *tok = new ConstantBoolean(yytext);
    tok->line = numOfLine;
    return addToken(tok);
}

int Lexer::addKeyword() {
    printf("found Keyword %s %d\n", yytext, xxx);
    warningf("todo!!!\n");
    return 1;
}

int Lexer::addMarker() {
    printf("found Marker %s %d \t in line %d \n", yytext, xxx, numOfLine);
    Marker *tok = new Marker(yytext);
    tok->line = numOfLine;
    return addToken(tok);
}

int Lexer::addChar() {
    printf("found Character %s %d \t in line %d \n", yytext, xxx, numOfLine);
    ConstantChar *tok = new ConstantChar(yytext);
    tok->line = numOfLine;
    return addToken(tok);
}

int Lexer::addLine() {
    numOfLine++;
    return 1;
}
