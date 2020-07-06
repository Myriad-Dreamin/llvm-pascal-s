//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::ParamList *RecursiveParser<Lexer>::parse_param_list_with_paren() {

    // (
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    auto lp = current_token;
    next_token();

    // param list
    auto param_list = parse_param_list();
    if (param_list == nullptr) {
        return nullptr;
    }

    // )
    if (!predicate::is_rparen(current_token)) {
        delete param_list;
        errors.push_back(new PascalSParseExpectVGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    auto rp = current_token;
    next_token();

    ast::copy_pos_between_tokens(param_list, lp, rp);
    return param_list;
}

template<typename Lexer>
ast::ParamList *RecursiveParser<Lexer>::parse_param_list() {
    //look ahead
    ast::ParamList *pl = _parse_param_list(new ast::ParamList);
    if (pl->params.empty()) {
        ast::copy_pos_with_check(pl, current_token);
        pl->length = 0;
    } else {
        ast::copy_pos_between_tokens(pl, pl->params.front(), pl->params.back());
    }
    return pl;
}


template<typename Lexer>
ast::ParamList *RecursiveParser<Lexer>::_parse_param_list(ast::ParamList *params) {
    for (;;) {
        auto spec = parse_param();
        if (spec != nullptr) {
            params->params.push_back(spec);
        }


        //look ahead
        // ;
        for (;;) {
            if (current_token == nullptr || predicate::is_rparen(current_token)) {
                return params;
            }
            if (predicate::is_semicolon(current_token)) {
                break;
            }
            skip_any_but_eof_token_s("param list sep marker ';' or ')'");
            return fall_expect_s("param list sep marker ';' or ')'"), params;
        }
        next_token();
    }
}

template<typename Lexer>
ast::ParamSpec *RecursiveParser<Lexer>::parse_param() {

    for (;;) {
        if (current_token == nullptr) {
            return fall_expect_s("keyword var or id list");
        }
        if (predicate::is_rparen(current_token)) {
#ifndef NOT_ALLOW_EMPTY
            return nullptr;
#else
            using pascal_s::todo = nullptr;
            return pascal_s::todo;
//            return fall_expect_s("keyword var or id list");
#endif
        }
        if (predicate::is_semicolon(current_token)) {
            return fall_expect_s("keyword var or id list");
        }
        if (predicate::is_var(current_token)) {
            break;
        }
        if (current_token->type == TokenType::Identifier) {
            break;
        }

        skip_any_but_eof_token_s("keyword var or id list");
        return fall_expect_s("keyword var or id list");
    }


    auto keyword_var = current_token;
    if (keyword_var->type != KeywordType::Var) {

        keyword_var = nullptr;
    } else {
        next_token();
    }

    // id list
    auto id_list = parse_id_list();
    if (id_list == nullptr) {
        return nullptr;
    }

    // :
    expected_enum_type(predicate::is_colon, predicate::marker_colon);
    next_token();


    if (!predicate::is_keyword(current_token)) {
        for (;;) {
            if (current_token == nullptr) {
                return fall_expect_s("basic type spec"), new ast::ParamSpec(keyword_var, id_list, nullptr);
            }
            if (predicate::is_rparen(current_token)) {
                return fall_expect_s("basic type spec"), new ast::ParamSpec(keyword_var, id_list, nullptr);
            }
            if (predicate::is_semicolon(current_token)) {
                return fall_expect_s("basic type spec"), new ast::ParamSpec(keyword_var, id_list, nullptr);
            }
            maybe_recover_keyword(KeywordType::Boolean)
            maybe_recover_keyword(KeywordType::Integer)
            maybe_recover_keyword(KeywordType::Char)
            maybe_recover_keyword(KeywordType::Real)
            skip_error_token_s("basic type spec")
            return fall_expect_s("basic type spec"), new ast::ParamSpec(keyword_var, id_list, nullptr);
        }
    }

    auto basic = current_token;
    auto spec = new ast::BasicTypeSpec(basic);
    next_token();

    return new ast::ParamSpec(keyword_var, id_list, spec);
}
