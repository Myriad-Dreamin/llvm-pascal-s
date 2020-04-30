
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
    std::cout
            << (fmt::format("found identifier {} {} \t in line {}\n", yytext, current_offset - line_offset, yylineno));
    auto tok = new Identifier(yytext);

    return addToken(tok);
}

int Lexer::addReal() {
    std::cout << (fmt::format("found Real {} {} \t in line {}\n", yytext, current_offset - line_offset, yylineno));
    auto tok = new ConstantReal(yytext);
    return addToken(tok);
}

int Lexer::addInteger() {
    std::cout << (fmt::format("found Integer {} {} \t in line {}\n", yytext, current_offset - line_offset, yylineno));
    auto tok = new ConstantInteger(yytext);
    return addToken(tok);
}

int Lexer::addBoolean() {
    std::cout << (fmt::format("found Boolean {} {} \t in line {}\n", yytext, current_offset - line_offset, yylineno));
    auto tok = new ConstantBoolean(yytext);
    return addToken(tok);
}

int Lexer::addKeyword() {
    std::cout << (fmt::format("found Keyword {} {} \t in line {} \n", yytext, current_offset - line_offset, yylineno));
    warningf("todo!!!\n");
    return 1;
}

int Lexer::addMarker() {
    std::cout << (fmt::format("found Marker {} {} \t in line {} \n", yytext, current_offset - line_offset, yylineno));
    auto tok = new Marker(yytext);
    return addToken(tok);
}

int Lexer::addChar() {
    std::cout
            << (fmt::format("found Character {} {} \t in line {} \n", yytext, current_offset - line_offset, yylineno));
    auto tok = new ConstantChar(yytext);
    return addToken(tok);
}

int Lexer::recordNewLine() {
    line_offset = current_offset;
    return 1;
}
