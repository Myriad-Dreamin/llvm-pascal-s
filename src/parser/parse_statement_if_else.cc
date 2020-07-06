//
// Created by kamiyoru on 2020/6/30.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::Statement *RecursiveParser<Lexer>::parse_if_else_statement(std::set<const Token *> *till) {
    auto *if_else = new ast::IfElseStatement();

    // if
    expected_enum_type_r(predicate::is_if, predicate::keyword_if, if_else);
    auto if_tok = current_token;
    next_token();

    // if cond
    if (till == nullptr) {
        std::set<const Token *> m_till;
        m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_then));
        if_else->cond = parse_exp(&m_till);
    } else {
        bool no_then = !till->count(&predicate::keyword_then);
        if (no_then) {
            till->insert(&predicate::keyword_then);
        }
        if_else->cond = parse_exp(till);
        if (no_then) {
            till->erase(&predicate::keyword_then);
        }
    }

    // then
    expected_enum_type_r(predicate::is_then, predicate::keyword_then, if_else);
    next_token();

    // if stmt
    if (till == nullptr) {
        std::set<const Token *> m_till;
        m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_else));
        if_else->if_part = parse_statement(&m_till);
    } else {
        bool no_else = !till->count(&predicate::keyword_else);
        if (no_else) {
            till->insert(&predicate::keyword_else);
        }
        if_else->if_part = parse_statement(till);
        if (no_else) {
            till->erase(&predicate::keyword_else);
        }
    }

    // else part
    if (predicate::is_else(current_token)) {
        next_token();

        // else stmt
        if_else->else_part = parse_statement(till);
    }

    ast::copy_pos_between_tokens(if_else, if_tok, if_else->else_part ? if_else->else_part : if_else->if_part);
    return if_else;
}


