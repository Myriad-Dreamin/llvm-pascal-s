//
// Created by kamiyoru on 2020/6/30.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::Statement *RecursiveParser<Lexer>::parse_for_statement(std::set<const Token *> *till) {
    auto *for_stmt = new ast::ForStatement();

    // for
    expected_enum_type_r(predicate::is_for, predicate::keyword_for, for_stmt);
    auto for_tok = current_token;
    next_token();

    // id
    expected_type_r(TokenType::Identifier, for_stmt);
    for_stmt->loop_var = reinterpret_cast<const Identifier *>(current_token);
    next_token();

    // :=
    expected_enum_type_r(predicate::is_assgin, predicate::marker_assgin, for_stmt);
    next_token();

    // from exp
    if (till == nullptr) {
        std::set<const Token *> m_till;
        m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_to));
        for_stmt->from_exp = parse_exp(&m_till);
    } else {
        bool no_to = !till->count(&predicate::keyword_to);
        if (no_to) {
            till->insert(&predicate::keyword_to);
        }
        for_stmt->from_exp = parse_exp(till);
        if (no_to) {
            till->erase(&predicate::keyword_to);
        }
    }

    // to
    expected_enum_type_r(predicate::is_to, predicate::keyword_to, for_stmt);
    next_token();

    // to exp
    if (till == nullptr) {
        std::set<const Token *> m_till;
        m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_do));
        for_stmt->to_exp = parse_exp(&m_till);
    } else {
        bool no_do = !till->count(&predicate::keyword_do);
        if (no_do) {
            till->insert(&predicate::keyword_do);
        }
        for_stmt->to_exp = parse_exp(till);
        if (no_do) {
            till->erase(&predicate::keyword_do);
        }
    }

    // do
    expected_enum_type_r(predicate::is_do, predicate::keyword_do, for_stmt);
    next_token();


    // for statement body
    for_stmt->for_stmt = parse_statement(till);

    ast::copy_pos_between_tokens(for_stmt, for_tok, for_stmt->for_stmt);
    return for_stmt;
}
