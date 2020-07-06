

%option c++
%option yyclass="Lexer"
%option noyywrap
%option yylineno

%{
#include <pascal-s/lexer.h>
#include "gulp.h"
%}

%x COMMENT COMMENT_ESCAPE


%%
\{ { comment_embed++; BEGIN COMMENT; yymore(); }
<COMMENT><<EOF>> { BEGIN INITIAL; comment_embed = 0; return addComment(), 0; }
<COMMENT_ESCAPE><<EOF>> { BEGIN INITIAL;  comment_embed = 0; return addComment(), 0; }

<COMMENT>\{ { comment_embed++; yymore(); }
<COMMENT>\\ { BEGIN COMMENT_ESCAPE; yymore(); }
<COMMENT_ESCAPE>(.|\n) { BEGIN COMMENT; yymore(); }
<COMMENT>\} { if (comment_embed > 0) comment_embed--;
 if (comment_embed == 0) { BEGIN INITIAL; return addComment(); } else yymore(); }
<COMMENT>[^\\{}]* { yymore(); }
\n { return recordNewLine(); }
\/\/.*\n? { return addInlineComment(); } // new line or EOF
(?i:to|do|of|for|else|if|then|char|boolean|real|integer|array|end|begin|function|procedure|var|const|program|read|write) { return addKeyword(); }
(?i:div|mod|and|or|not) { return addMarker(); }
\<>|\<=|>=|:=|\.\.|[()\[\]<=>,.;:+\-*/] { return addMarker(); }
% { return addMarker(); }
true|false { return addBoolean(); }
[\_a-zA-Z][\_a-zA-Z0-9]* { return addIdentifier(); }
[0-9]*\.[0-9]+ { return addReal(); }
[0-9]+ { return addInteger(); }
'(\\'|[^'])*' { return addChar(); }

[\_a-zA-Z0-9]* { return skipErrorString("lexical error: identifier started with number is not allow"); }
[^[:space:]] { return skipErrorString("lexical error: unknown character in input stream"); }
[[:space:]] {} /* do nothing */

%%

