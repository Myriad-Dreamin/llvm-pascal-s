

%option c++
%option yyclass="Lexer"
%option noyywrap

%{
#include <pascal-s/lexer.h>
%}


%%
to|do|of|for|else|if|then|char|boolean|real|integer|array|end|begin|function|procedure|var|const|program { return addKeyword(); }

true|false { return addBoolean(); }
[a-zA-Z][a-zA-Z0-9]* { return addIdentifier(); }
[0-9]*\.[0-9]+ { return addReal(); }
[0-9]+\.[0-9]* { return addReal(); }
[0-9]+ { return addInteger(); }
'.*' { return addChar(); }

\<>|\<=|>=|:=|\.\.|[()\[\]<=>,.;:+\-*/] { return addMarker(); }

%%

