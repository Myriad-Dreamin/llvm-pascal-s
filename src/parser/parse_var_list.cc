//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::VariableList *RecursiveParser<Lexer>::parse_variable_list_with_paren() {

    // (
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    auto lp = current_token;
    next_token();

    // variable list
    auto var_list = parse_variable_list();
    if (var_list == nullptr) {
        return nullptr;
    }

    // )
    if (!predicate::is_rparen(current_token)) {
        delete var_list;
        errors.push_back(new PascalSParseExpectVGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    auto rp = current_token;
    next_token();

    ast::copy_pos_between_tokens(var_list, lp, rp);
    return var_list;
}

template<typename Lexer>
ast::VariableList *RecursiveParser<Lexer>::parse_variable_list() {
    auto *ret = new ast::VariableList;
    for (;;) {

        for (;;) {
            // look ahead
            if (predicate::is_rparen(current_token)) {
                goto parse_end;
            }
            if (current_token != nullptr && current_token->type == TokenType::Identifier) {
                break;
            }
            skip_error_token_s("variable list right paren");
            return fall_expect_s("variable list right paren"), ret;
        }


        // extend production
        ret->params.push_back(parse_variable(&predicate::predicateContainers.commaOrRParenContainer));

        for (;;) {
            if (predicate::token_equal(current_token, &predicate::predicateContainers.commaOrRParenContainer)) {
                break;
            }
            skip_error_token_s("variable list sep marker ',' or ')'");
            return fall_expect_s("variable list sep marker ',' or ')'"), ret;
        }

        // eat , if possible
        if (predicate::is_comma(current_token)) {
            next_token();

            // want FOLLOW(variable) = {)}
        } else if (!predicate::is_rparen(current_token)) {
            goto parse_end;
        }
    }
    parse_end:
    if (ret->params.empty()) {
        ast::copy_pos_with_check(ret, current_token);
        ret->length = 0;
    } else {
        ast::copy_pos_between_tokens(ret, ret->params.front(), ret->params.back());
    }
    return ret;
}

