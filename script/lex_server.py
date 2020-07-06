# please move py_pascal_s.cpXX-win_YYY.pyd to your python/src/DLLs
# cmake-build-debug/src/py-module/py_pascal_s.cp37-win_amd64.pyd
#
import json
import os
import sys
from typing import List

import html
import py_pascal_s


def reinterpret_pascal_s_token(tok_ptr):
    if tok_ptr.type == py_pascal_s.PascalSTokenType.Keyword:
        return py_pascal_s.reinterpret_as_keyword(tok_ptr)  # type: py_pascal_s.PascalSTokenKeyword
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.Marker:
        return py_pascal_s.reinterpret_as_marker(tok_ptr)  # type: py_pascal_s.PascalSTokenMarker
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.ConstantString:
        return py_pascal_s.reinterpret_as_constant_string(tok_ptr)  # type: py_pascal_s.PascalSTokenConstantString
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.Comment:
        return py_pascal_s.reinterpret_as_comment(tok_ptr)  # type: py_pascal_s.PascalSTokenComment
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.ErrorToken:
        return py_pascal_s.reinterpret_as_error_token(tok_ptr)  # type: py_pascal_s.PascalSTokenErrorToken
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.ConstantBoolean:
        return py_pascal_s.reinterpret_as_constant_boolean(tok_ptr)  # type: py_pascal_s.PascalSTokenConstantBoolean
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.ConstantInteger:
        return py_pascal_s.reinterpret_as_constant_integer(tok_ptr)  # type: py_pascal_s.PascalSTokenConstantInteger
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.ConstantReal:
        return py_pascal_s.reinterpret_as_constant_real(tok_ptr)  # type: py_pascal_s.PascalSTokenConstantReal
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.ConstantChar:
        return py_pascal_s.reinterpret_as_constant_char(tok_ptr)  # type: py_pascal_s.PascalSTokenConstantChar
    elif tok_ptr.type == py_pascal_s.PascalSTokenType.Identifier:
        return py_pascal_s.reinterpret_as_identifier(tok_ptr)  # type: py_pascal_s.PascalSTokenIdentifier
    return tok_ptr


class PascalSLexerRenderer(object):
    html_template = """
<html lang="en">
<head>
<title>Pascal-S-Lexer-View</title>
<meta charset="UTF-8"/>
<style>
    .const-char {
            color: #7ec699;
    }
    .const-integer {
            color: #49a8f0;
    }

    .const-boolean {
            color: #f8c555;
    }

    .identifier {
            color: #f08d49;
    }
    
    .error-token {
            color: #ff6347;
    }

    .keyword {
            color: #cc99cd;
    }
    
    .comment {
            color: #999;
    }

    .marker {
        color: #ccc;
    }

    .source-code {
        background: #2d2d2d;
        padding: 0 20px;
    }
    .lc {
        width: 8vw;
    }
    .t-type {
        width: 25vw;
    }
    .lc, .t-type {
        display: inline-block;
    }
    body  p {
        font-size: 1.5vw;
    }
    #source-mapping {
        overflow-x:scroll;
        max-height: 50vh;
    }
</style>
</head>
<body>
<div class='source-code'><pre class="language-pascal-s"><code class="language-pascal-s">{{content}}</code></pre></div>
<div id='source-mapping'></div>
</body>
<script>
    window.tokens = 'token_definition_replacement';
    let source_mapping = '';
    // noinspection JSUnresolvedVariable
    for(let i in window.tokens) {
    // noinspection JSUnfilteredForInLoop, JSUnresolvedVariable
    let tok = window.tokens[i];
    switch (tok.type) {
    case "PascalSTokenType.Keyword":
        document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, key_type = ${tok['key_type']}</p>`)
                ;
            break;
        case "PascalSTokenType.Marker":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, marker_type = ${tok['marker_type']}</p>`) 
               ;
            break;
        case "PascalSTokenType.Identifier":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, content = ${tok['content']}</p>`);
            break;
        case "PascalSTokenType.ConstantString":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, attr = ${tok['attr']}</p>`);
            break;
        case "PascalSTokenType.Comment":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, content = ${tok['content']}</p>`);
            break;
        case "PascalSTokenType.ErrorToken":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`
             + (tok['hint'] === '' ? '' : (' ' + tok['hint']));
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}` + 
                `</span>, content = ${tok['content']}, hint = ${tok['hint']}</p>`);
            break;
        case "PascalSTokenType.ConstantBoolean":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, attr = ${tok['attr']}</p>`);
            break;
        case "PascalSTokenType.ConstantInteger":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, attr = ${tok['attr']}</p>`);
            break;
        case "PascalSTokenType.ConstantReal":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, attr = ${tok['attr']}</p>`);
            break;
        case "PascalSTokenType.ConstantChar":
            document.getElementById(`token-ref-${i}`).title = `${tok['offset']}:${tok['line']}:${tok['column']}`;
            source_mapping += (
                `<p><span class='lc'>${tok['offset']}:${tok['line']}:${tok['column']}:</span>` + 
                `<span class='t-type'>type = ${tok['type']}</span>, attr = ${tok['attr']}</p>`);
            break;
            default:
            console.error(JSON.stringify(tok));
            break;
            }
      }
        document.getElementById('source-mapping').innerHTML = source_mapping;
</script>
</html>"""

    @staticmethod
    def half_serialize_tokens(tokens):
        dicts = []

        for i, tok in enumerate(tokens):

            tok_dict = {"type": str(tok.type), "offset": tok.offset, "length": tok.length, "line": tok.line,
                        "column": tok.column + 1}
            dicts.append(tok_dict)

            if tok.type == py_pascal_s.PascalSTokenType.Keyword:
                tok_dict['key_type'] = str(tok.key_type)
            elif tok.type == py_pascal_s.PascalSTokenType.Marker:
                tok_dict['marker_type'] = str(tok.marker_type)
            elif tok.type == py_pascal_s.PascalSTokenType.Identifier:
                tok_dict['content'] = tok.content
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantString:
                tok_dict['attr'] = tok.attr
            elif tok.type == py_pascal_s.PascalSTokenType.Comment:
                tok_dict['content'] = tok.content
            elif tok.type == py_pascal_s.PascalSTokenType.ErrorToken:
                tok_dict['content'] = tok.content
                tok_dict['hint'] = tok.hint
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantBoolean:
                tok_dict['attr'] = tok.attr
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantInteger:
                tok_dict['attr'] = tok.attr
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantReal:
                tok_dict['attr'] = tok.attr
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantChar:
                tok_dict['attr'] = tok.attr

        return dicts

    @staticmethod
    def render_source(source_stream, tokens):
        str_slice = []

        tok_len = len(tokens)
        if tok_len != 0:
            end_first = next(reversed(tokens))
            str_slice.append(source_stream[end_first.offset + end_first.length:])
            source_stream = source_stream[:end_first.offset + end_first.length]

        for i, tok in enumerate(reversed(tokens)):
            str_slice.append(html.escape(source_stream[tok.offset + tok.length:]))
            if tok.type == py_pascal_s.PascalSTokenType.Keyword:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token keyword">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.Marker:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token marker">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.Identifier:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token identifier">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantString:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token const-string">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.Comment:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token comment">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.ErrorToken:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token error-token">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantBoolean:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token const-boolean">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantInteger:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token const-integer">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantReal:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token const-real">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
            elif tok.type == py_pascal_s.PascalSTokenType.ConstantChar:
                str_slice.append(
                    f'<span id="token-ref-{tok_len - 1 - i}" class="token const-char">{source_stream[tok.offset:tok.offset + tok.length]}</span>')
                source_stream = source_stream[:tok.offset]
        str_slice.append(html.escape(source_stream))

        return ''.join(reversed(str_slice))

    def render(self, source_path, target_path):
        return self.renders(open(source_path).read(), target_path)

    @staticmethod
    def lex(source):
        lexer = py_pascal_s.PascalSLexer()
        toks = list(map(reinterpret_pascal_s_token, lexer.lex(source)))  # type: List[py_pascal_s.PascalSToken]
        return lexer, toks

    def renders(self, source, target_path):
        handler, toks = self.lex(source)

        rendered_string = self.render_source(source, toks)
        tok_dicts = self.half_serialize_tokens(toks)

        with open(target_path, 'w') as f:
            f.write(PascalSLexerRenderer.html_template.replace("{{content}}", rendered_string)
                    .replace("'token_definition_replacement'", json.dumps(tok_dicts)))
        del handler


if __name__ == '__main__':
    source = None
    target = None
    if len(sys.argv) == 2:
        source = sys.argv[1]

    if len(sys.argv) == 3:
        source = sys.argv[1]
        target = sys.argv[2]

    if len(sys.argv) <= 1:
        exit(1)
        # test
        PascalSLexerRenderer().renders("""program main(input,output);
var
    n,i:integer;
    list:array[0..1000] of integer;
    c:char;
procedure qsort(low, high:integer);
var
    11l,h,22m:integer;
    i,j:integer;
    temp:integer;
    flag:integer;
begin
    flag:=0;  #       ???????????
    l:=low; h:=high;
    m:=list[(l+h) div 2];
    for i:=1 to 1000 do
    begin
    if flag=0 then begin
        for j:=1 to 1000 do
            if list[l]<m then l:=l+1;
        for j:=1 to 1000 do
            if list[h]>m then h:=h-1;
        if l<=h then
        begin
            temp:=list[l]; list[l]:=list[h]; list[h]:=temp;
            l:=l+1; h:=h-1;
        end;
        if (l>h) then flag:=1;
    end;
    end;
    if l<high then qsort(l,high);
    if h>low then qsort(low,h);
end; {qsort end}

begin
    read(n);
    for i:=0 to n-1 do
        read(list[i]);
    qsort(0,n-1);
        for i:=0 to n-1 do
        begin
            write(list[i], '\\n', '\\x233');
        end
end. {main end}
    """, os.path.expanduser('~/Desktop/test.html'))
    else:
        PascalSLexerRenderer().render(source, target or os.path.expanduser('~/Desktop/test.html'))
