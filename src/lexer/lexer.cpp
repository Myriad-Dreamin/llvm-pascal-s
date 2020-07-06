
#include <pascal-s/logger.h>
#include <pascal-s/lib/convert.h>
#include <pascal-s/token.h>
#include <pascal-s/lexer.h>

#include <iostream>
#include <string_view>
#include <algorithm>

/*
 * Lexer 构造函数与析构函数
 */

Lexer::Lexer(std::istream *in, std::ostream *out)
        : yyFlexLexer(in, out) {}

Lexer::~Lexer() = default;


/*
 * Lexer 解析regex匹配到的各种字符串
 */

/*
 * addIdentifier 在token流后追加一个Identifier
 *
 * 将regex文本大小写全转为小写
 * 将regex文本装入Identifier
 */
int Lexer::addIdentifier() {
    std::string str_temp = std::string(yytext);
    std::transform(str_temp.begin(), str_temp.end(), str_temp.begin(), tolower);
    auto tok = new Identifier(str_temp.c_str());
    return addToken(tok);
}

/*
 * addReal 在token流后追加一个ConstantReal
 *
 * 将regex文本转换为pascal_s_real_t
 * 将数值装入ConstantReal
 * 由于精度损失，原字符串也会一并传入ConstantReal
 */
int Lexer::addReal() {

    auto tok = new ConstantReal(yytext, std::strtod(yytext, nullptr));
    return addToken(tok);
}

/*
 * addInteger 在token流后追加一个ConstantInteger
 *
 * 将regex文本转换为pascal_s_integer_t
 * 将数值装入ConstantInteger
 */
int Lexer::addInteger() {
    auto tok = new ConstantInteger(std::strtoll(yytext, nullptr, 10));
    return addToken(tok);
}

/*
 * addBoolean 在token流后追加一个ConstantBoolean
 *
 * 将regex文本转换为bool
 * 将数值装入ConstantBoolean
 */
int Lexer::addBoolean() {
    auto tok = new ConstantBoolean(parseBool(yytext));
    return addToken(tok);
}

/*
 * addBoolean 在token流后追加一个ConstantBoolean
 *
 * 将regex文本转换为bool
 * 将数值装入ConstantBoolean
 */
int Lexer::addKeyword() {
    std::string str_temp = std::string(yytext);
    std::transform(str_temp.begin(), str_temp.end(), str_temp.begin(), tolower);
    auto tok = new Keyword(get_keyword_type(str_temp));
    return addToken(tok);
}


/*
 * addMarker 在token流后追加一个Marker
 *
 * 将regex文本转换marker_type
 * 将类型装入Marker
 */
int Lexer::addMarker() {
    std::string str_temp = std::string(yytext);
    std::transform(str_temp.begin(), str_temp.end(), str_temp.begin(), tolower);
    auto tok = new Marker(get_marker_type(str_temp));
    return addToken(tok);
}

/*
 * char_spec 保存全局的char映射信息
 *
 * 包含变量spec_char_map
 *   + [ch]: 若\ch是一个合法的单字符转义文本，则返回对应的char类型值
 *   + count: 返回ch映射是否存在于表中
 * 表中包含\n, \t, \r, \\
 *
 * 包含变量hex_table
 *   + [ch]: 若\ch是一个合法的16进制数字，则返回对应的16进制数值
 *   + count: 返回ch映射是否存在于表中
 * 表中包含0-9, a-f, A-F
 */
namespace char_spec {
    static const char invalid = '0';

    static struct _spec_char_map_t {
        char inner[256]{};

        explicit _spec_char_map_t() noexcept {
            for (char &i : inner) {
                i = invalid;
            }

            // \n, \t, \r, \\

            inner['a'] = '\a';
            inner['b'] = '\b';
            inner['f'] = '\f';
            inner['n'] = '\n';
            inner['r'] = '\r';
            inner['t'] = '\t';
            inner['v'] = '\v';
            inner['\''] = '\'';
            inner['\\'] = '\\';

            // \0 ~ \9
            for (int i = 0; i < 10; i++) {
                inner['0' + i] = char(i);
            }
        }

        char operator[](char a) {
            assert(count(a));
            return inner[a];
        }

        bool count(char a) {
            return inner[a] != invalid;
        }
    } spec_char_map;

    static struct _hex_table_t {
        char inner[256]{};

        explicit _hex_table_t() noexcept {
            for (char &i : inner) {
                i = invalid;
            }
            for (int i = 0; i < 10; i++) {
                inner[i + '0'] = char(i);
            }
            for (int i = 0; i < 6; i++) {
                inner[i + 'a'] = char(10 + i);
                inner[i + 'A'] = char(10 + i);
            }
        }

        char operator[](char a) {
            assert(count(a));
            return inner[a];
        }

        bool count(char a) {
            return inner[a] != invalid;
        }
    } hex_table;
}

/*
 * 返回ch是否是一个合法的16进制单字符
 */
static int ishex(char ch) {
    return char_spec::hex_table.count(ch);
}

/*
 * addASCIIChar 如果regex文本是一个合法的ascii字符（包括转义）, 则在token流后追加一个ConstantChar
 * 否则返回0
 */
int Lexer::addASCIIChar() {

    // from this line, yytext.length >= 0, safe index: 0

    //                0
    // case1: any but '
    //        0 1
    // case2: ' $
    if (yytext[0] != '\'' || yytext[1] == '\00') {
        return static_cast<lexer_action_code_underlying_type>(LexerActionCode::LexEnd);
    }

    // from this line, yytext.length >= 2, safe index: 0-2

    //        0   1 2 3
    // case3: ' any ' $
    if (yytext[2] == '\'' && yytext[3] == '\00') {
        return addToken(new ConstantChar(yytext[1]));
    }

    // from this line, yytext.length >= 2, safe index: 0-2


    //        0 1
    // case4: ' \

    if (yytext[1] == '\\') {
        // match escape rules with priority

        //                          2 3 4
        // case4.1: valid escape char ' $
        if (char_spec::spec_char_map.count(yytext[2]) && yytext[3] == '\'' && yytext[4] == '\00') {
            return addToken(new ConstantChar(char_spec::spec_char_map[yytext[2]]));
        }

        //          2   3   4 5 6
        // case4.2: x hex hex ' $
        if (yytext[2] == 'x' && ishex(yytext[3]) && ishex(yytext[4]) && yytext[5] == '\'' && yytext[6] == '\00') {
            return addToken(
                    new ConstantChar(char(char_spec::hex_table[yytext[3]] * 16 + char_spec::hex_table[yytext[4]])));
        }

        //            2   3 4 5
        // case4.3: hex hex ' $
        if (ishex(yytext[2]) && ishex(yytext[3]) && yytext[4] == '\'' && yytext[5] == '\00') {
            return addToken(
                    new ConstantChar(char(char_spec::hex_table[yytext[2]] * 16 + char_spec::hex_table[yytext[3]])));
        }
    }


    return static_cast<lexer_action_code_underlying_type>(LexerActionCode::LexEnd);
}

const static auto CharErrorHint = "lexer error: not a valid char";
const static auto CommentNotCloseErrorHint = "lexer error: not closed cross line comment";

/*
 * addChar 在token流后追加一个ConstantChar
 *
 * 如果转换失败，在错误流后追加一个错误
 */
int Lexer::addChar() {
    int code = addASCIIChar();
    // unicode rule can be extend
    // code = code ? code : addUnicodeChar();
    if (!code) {
        auto tok = ErrorToken::copy_in(yytext, CharErrorHint);
        return addError(tok), addToken(tok);
    }
    return code;
}

/*
 * skipErrorString 在错误流后追加一个错误
 */
int Lexer::skipErrorString(const char *hint) {
    auto tok = ErrorToken::copy_in(yytext, hint);
    return addError(tok), addToken(tok);
}

/*
 * recordNewLine 更新行偏移
 */
int Lexer::recordNewLine() {
    line_offset = current_offset;
    return static_cast<lexer_action_code_underlying_type>(LexerActionCode::AuxFunctionCalled);
}

/*
 * 将转义符号转为正确的符号
 */
char *removeSkipRule(char *content, int yyleng) {
    int j = 0;
    for (int i = 0; i < yyleng; i++, j++) {
        if (content[i] == '\\') {
            i++;
        }
        content[j] = content[i];
    }
    content[j] = 0;
    return content;
}

/*
 * addComment 在token流后追加一个Cross Line Comment（Option）
 * 掩码为LexerOptionLexComment
 *
 * 如果发现comment未封闭，则产生一个错误
 */
int Lexer::addComment() {
    current_offset += yyleng - 1;

    int lineno_recover = yylineno;

    for (int i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            yylineno--;
        }
    }

    int code = 0;
    if (yytext[yyleng - 1] != '}') {
        auto err = new ErrorToken(removeSkipRule(yytext, yyleng), CommentNotCloseErrorHint);
        addError(err);
        err->line = yylineno;
        err->length = yyleng - 1;
        err->column = current_offset - yyleng - line_offset;
        err->offset = current_offset - yyleng;
    }
    code = addCommentAux();

    yylineno = lineno_recover;

    for (int i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            line_offset = current_offset - yyleng + 1 + i;
        }
    }
    return code;
}

/*
 * addInlineComment 在token流后追加一个Inline Comment （Option）
 *
 * 掩码为LexerOptionLexComment
 */
int Lexer::addInlineComment() {
    int code = addCommentAux();
    line_offset = current_offset;
    return code;
}

/*
 * 检查lexer选项，如果需要，则往token流中加入这个Comment
 * 掩码为LexerOptionLexComment
 */
int Lexer::addCommentAux() {
    if (option_mask & LexerOptionLexComment) {
        return addToken(new Comment(removeSkipRule(yytext, yyleng)));
    }
    return static_cast<lexer_action_code_underlying_type>(LexerActionCode::AuxFunctionCalled);
}
