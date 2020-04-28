
#include <pascal-s/token.h>
#include <cstdio>
#include <pascal-s/lexer.h>

int main() {

    Lexer lexer;

    while( lexer.yylex() );

    printf("lexer test");
}


