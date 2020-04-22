flex -o flex-lexer.h lexer.flex
sed flex-lexer.h -e 's/#define yyFlexLexer yyFlexLexer//g' > flex-lexer.h.tmp
mv flex-lexer.h.tmp flex-lexer.h