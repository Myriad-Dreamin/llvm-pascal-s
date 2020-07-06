//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::ExpressionList *RecursiveParser<Lexer>::parse_expression_list_with_paren() {

    // (
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    auto lp = current_token;
    next_token();

    // expression list
    auto exp_list = parse_expression_list(predicate::is_rparen, &predicate::predicateContainers.commaOrRParenContainer);
    if (exp_list == nullptr) {
        return nullptr;
    }

    // )
    expected_enum_type(predicate::is_rparen, predicate::marker_rparen);
    auto rp = current_token;
    next_token();
    ast::copy_pos_between_tokens(exp_list, lp, rp);
    return exp_list;
}

template<typename Lexer>
ast::ExpressionList *RecursiveParser<Lexer>::parse_expression_list_with_bracket() {

    // [
    expected_enum_type(predicate::is_lbracket, predicate::marker_lbracket);
    auto lp = current_token;
    next_token();

    // expression list
    auto exp_list = parse_expression_list(predicate::is_rbracket,
                                          &predicate::predicateContainers.commaOrRBracketContainer);
    if (exp_list == nullptr) {
        return nullptr;
    }

    // ]
    expected_enum_type(predicate::is_rbracket, predicate::marker_rbracket);
    auto rp = current_token;
    next_token();

    ast::copy_pos_between_tokens(exp_list, lp, rp);

    if (exp_list->explist.size() == 0) {
        errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "any expression", rp));
    }

    return exp_list;
}

template<typename Lexer>
template<typename F>
ast::ExpressionList *
RecursiveParser<Lexer>::parse_expression_list(const F &is_follow, const std::set<const Token *> *till) {
    auto *ret = new ast::ExpressionList;
    for (;;) {

        // look ahead
        if (is_follow(current_token)) {
            return ret;
        }

        // extend production
        ret->explist.push_back(parse_exp(till));

        // eat , if possible
        if (predicate::is_comma(current_token)) {
            next_token();

            // want FOLLOW(variable) = {)}
        } else if (!is_follow(current_token)) {
            return ret;
        }
    }
    return nullptr;
}

