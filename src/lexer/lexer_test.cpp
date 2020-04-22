
#include <token.h>
#include <cstdio>
#include "flex-lexer.h"

int main() {

    Lexer lexer;

    while( lexer.yylex() );

    printf("lexer test");
}


