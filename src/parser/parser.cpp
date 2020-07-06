//
// Created by kamiyoru on 2020/5/8.
//

#include <pascal-s/mock.h>
#include <pascal-s/parser_recursive.h>
#include <set>
#include <fmt/core.h>

#include "pascal_s_parser_adaptor.cc"

#define maybe_recover_keyword(K) if (guess_keyword(K)) {\
        break;\
    }

#define fall_expect_t(T) (errors.push_back(new PascalSParseExpectTGotError(__FUNCTION__, T, current_token)), nullptr)
#define fall_expect_s(T) (errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, T, current_token)), nullptr)
#define fall_expect_v(T) (errors.push_back(new PascalSParseExpectVGotError(__FUNCTION__, T, current_token)), nullptr)

#define skip_error_token(T, Tok) if (current_token != nullptr && current_token->type == TokenType::ErrorToken) {\
        errors.push_back(new PascalSParseExpect ## T ## GotError(__FUNCTION__, Tok, current_token));\
        next_token();\
        continue;\
    }

#define skip_error_token_t(Tok) skip_error_token(T, Tok)
#define skip_error_token_s(Tok) skip_error_token(S, Tok)
#define skip_error_token_v(Tok) skip_error_token(V, Tok)

#define skip_any_but_eof_token(T, Tok) if (current_token != nullptr) {\
        errors.push_back(new PascalSParseExpect ## T ## GotError(__FUNCTION__, Tok, current_token));\
        next_token();\
        continue;\
    }

#define skip_any_but_eof_token_t(Tok) skip_any_but_eof_token(T, Tok)
#define skip_any_but_eof_token_s(Tok) skip_any_but_eof_token(S, Tok)
#define skip_any_but_eof_token_v(Tok) skip_any_but_eof_token(V, Tok)

#define expected_enum_type_r_e(predicator, indicate, rvalue, ext) do {if (!predicator(current_token)) {\
    for(;;) {\
        if (predicator(current_token)) {\
            break;\
        }\
        if (!(predicate::is_end(current_token) || predicate::is_semicolon(current_token) ext)) {\
            skip_any_but_eof_token_v(&indicate)\
        }\
        errors.push_back(new PascalSParseExpectVGotError(__FUNCTION__, &indicate, current_token));\
        return rvalue;\
    }\
}}while(0)

#define expected_enum_type_r(predicator, indicate, rvalue) expected_enum_type_r_e(predicator, indicate, rvalue, )

#define expected_enum_type(predicator, indicate) expected_enum_type_r(predicator, indicate, nullptr)

#define expected_type_r_e(tok_type, rvalue, ext) do {if (current_token == nullptr || current_token->type != tok_type) {\
    for(;;) {\
        if (current_token != nullptr && current_token->type == tok_type) {\
            break;\
        }\
        if (!(predicate::is_end(current_token) || predicate::is_semicolon(current_token) ext)) {\
            skip_error_token_t(tok_type)\
        }\
        errors.push_back(new PascalSParseExpectTGotError(__FUNCTION__, tok_type, current_token));\
        return rvalue;\
    }\
}}while(0)

#define expected_type_r(tok_type, rvalue) expected_type_r_e(tok_type, rvalue, )
#define expected_type(tok_type) expected_type_r(tok_type, nullptr)

namespace predicate {
    struct _parserPredicateContainer {
        const std::set<const Token *> rParenContainer;
        const std::set<const Token *> semicolonContainer;
        const std::set<const Token *> semicolonOrVarContainer;
        const std::set<const Token *> commaOrRParenContainer;
        const std::set<const Token *> commaOrRBracketContainer;
        const std::vector<Token *> thenContainer;
        const std::vector<Token *> elseContainer;
        const std::vector<Token *> endOrSemicolonContainer;
        const std::vector<Token *> endOrSemicolonOrElseContainer;


        explicit _parserPredicateContainer() noexcept:
                rParenContainer(
                        {
                                reinterpret_cast<const Token *>(&predicate::marker_rparen)
                        }),
                semicolonContainer(
                        {
                                reinterpret_cast<const Token *>(&predicate::marker_semicolon)
                        }),
                semicolonOrVarContainer(
                        {
                                reinterpret_cast<const Token *>(&predicate::marker_semicolon),
                                reinterpret_cast<const Token *>(&predicate::keyword_var)
                        }),
                thenContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_then))
                        }),
                elseContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_else))
                        }),
                endOrSemicolonContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_semicolon)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_end)),
                        }),
                commaOrRParenContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_comma)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_rparen)),
                        }),
                commaOrRBracketContainer({
                                                 const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_comma)),
                                                 const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_rbracket)),
                                         }),
                endOrSemicolonOrElseContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_comma)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_rparen)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_else)),
                        }) {
        }
    } predicateContainers;
}

template<typename Lexer>
RecursiveParser<Lexer>::RecursiveParser(LexerProxy<Lexer> lexer) : lexer(std::move(lexer)) {}

template<typename Lexer>
const Token *RecursiveParser<Lexer>::next_token() {
    current_token = lexer.next_token();
    return current_token;
}

template<typename Lexer>
ast::Node *RecursiveParser<Lexer>::parse() {
    next_token();
    return parse_program();
}


template<typename Lexer>
bool RecursiveParser<Lexer>::guess_keyword(KeywordType k) {

    if (current_token == nullptr) {
        return false;
    }

    if (predicate::is_keyword_type(current_token->type) && current_token->type == k) {
        return true;
    }

    if (errors.size() > 50) {
        return false;
    }
    const std::string &k_str = get_keyword_type_reversed(k);

    if ((current_token->type == TokenType::Identifier &&
         guesser.near(k_str.c_str(), k_str.length(),
                      reinterpret_cast<const Identifier *>(current_token)->content,
                      current_token->length)) ||
        (current_token->type == TokenType::ErrorToken &&
         guesser.near(k_str.c_str(), k_str.length(), reinterpret_cast<const ErrorToken *>(current_token)->content,
                      current_token->length))) {
        errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "TokenType::Keyword", current_token,
                                                         " it may be keyword '" + k_str + "'"));
        update_guess(new Keyword(k));
        return true;
    }
    return false;
}

template<typename Lexer>
void RecursiveParser<Lexer>::update_guess(Token *new_tok) {
    copy_pos_any(new_tok, current_token);
    current_token = new_tok;
    this->guessing_token.push_back(new_tok);
}

template<typename Lexer>
bool RecursiveParser<Lexer>::has_error() {
    return !errors.empty();
}

template<typename Lexer>
RecursiveParser<Lexer>::~RecursiveParser() {
    for (auto e: errors) {
        delete e;
    }

    for (auto t: guessing_token) {
        deleteToken(t);
    }
}

#include "parse_program.cc"
#include "parse_program_head.cc"
#include "parse_program_body.cc"

#include "parse_subprogram_decls.cc"
#include "parse_subprogram_head.cc"
#include "parse_subprogram_body.cc"

#include "parse_const_decls.cc"
#include "parse_var_decls.cc"

#include "parse_id_list.cc"
#include "parse_expression_list.cc"
#include "parse_param_list.cc"
#include "parse_var_list.cc"

#include "parse_type.cc"
#include "parse_const_exp.cc"
#include "parse_variable.cc"
#include "parse_exp.cc"

#include "parse_statement.cc"
#include "parse_statement_compound.cc"
#include "parse_statement_for.cc"
#include "parse_statement_if_else.cc"

#undef expected_keyword
#undef expected_enum_type
#undef expected_enum_type_r
#undef expected_enum_type_r_e
#undef expected_type
#undef expected_type_r
#undef expected_type_r_e

#undef maybe_recover_keyword
#undef skip_error_token
#undef skip_any_but_eof_token

#undef fall_expect_t
#undef fall_expect_s
#undef fall_expect_v
#undef skip_error_token_t
#undef skip_error_token_s
#undef skip_error_token_v
#undef skip_any_but_eof_token_t
#undef skip_any_but_eof_token_s
#undef skip_any_but_eof_token_v