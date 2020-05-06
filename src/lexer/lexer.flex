

%option c++
%option yyclass="Lexer"
%option noyywrap
%option yylineno

%{
#include <pascal-s/lexer.h>
#include "gulp.h"
%}


%%
\{[^}]*\} {} /* do nothing */
to|do|of|for|else|if|then|char|boolean|real|integer|array|end|begin|function|procedure|var|const|program { return addKeyword(); }

true|false { return addBoolean(); }
[\_a-zA-Z][\_a-zA-Z0-9]* { return addIdentifier(); }
[0-9]*\.[0-9]+ { return addReal(); }
[0-9]+ { return addInteger(); }
'.*' { return addChar(); }
\n { return recordNewLine(); }

\<>|\<=|>=|:=|\.\.|[()\[\]<=>,.;:+\-*/] { return addMarker(); }

%%

