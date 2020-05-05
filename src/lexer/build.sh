out=flex-lexer.cpp

flex -o ${out} lexer.flex
sed ${out} -e 's/#define yyFlexLexer yyFlexLexer//g' \
  -e 's/#include <inttypes.h>/#include <cstdint>/g' > ${out}.tmp
mv ${out}.tmp ${out}