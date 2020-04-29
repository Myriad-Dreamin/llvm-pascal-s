out=flex-lexer.cpp

flex -o ${out} lexer.flex
sed ${out} -e 's/#define yyFlexLexer yyFlexLexer//g' > ${out}.tmp
mv ${out}.tmp ${out}