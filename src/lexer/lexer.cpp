
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
    clear_latest();
    for (auto &token :tokens) {
        deleteToken(token);
    }
}

int Lexer::addToken(Token *token) {
    tokens.push_back(token);
    return 1;
}

int Lexer::addIdentifier() {
    clear_latest();
    int l = strlen(yytext);
    latest = new char[l];
    strcpy(latest, yytext);
    return 1;
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

void Lexer::clear_latest() {
    if (latest) {
        delete[] latest;
        latest = nullptr;
    }
}

const Token *Lexer::next_token() {
    throw std::runtime_error("todo");
}

const Lexer::token_container &Lexer::get_all_tokens() {
    throw std::runtime_error("todo");
}


