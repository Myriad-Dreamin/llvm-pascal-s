//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::SubprogramDecls *RecursiveParser<Lexer>::parse_subprogram_decls() {
    return _parse_subprogram_decls(new ast::SubprogramDecls);
}

template<typename Lexer>
ast::SubprogramDecls *RecursiveParser<Lexer>::_parse_subprogram_decls(ast::SubprogramDecls *decls) {

    // function head

    auto subprogram = new ast::Subprogram(nullptr, nullptr);
    decls->vec.push_back(subprogram);

    // subprogram head
    subprogram->head = parse_subprogram_head();
    if (subprogram->head == nullptr) {
        return decls;
    }

    // ;
    expected_enum_type_r(predicate::is_semicolon, predicate::marker_semicolon, decls);
    subprogram->semicolon = current_token;
    next_token();

    // subprogram body
    subprogram->body = parse_subprogram_body();
    if (subprogram->body == nullptr) {
        return decls;
    }

    // ;
    expected_enum_type_r(predicate::is_semicolon, predicate::marker_semicolon, decls);
    subprogram->semicolon = current_token;
    next_token();

    ast::copy_pos_between_tokens(subprogram, subprogram->head, subprogram->semicolon);

    // look ahead
    if (predicate::is_function(current_token) || predicate::is_procedure(current_token)) {
        return _parse_subprogram_decls(decls);
    }

    return decls;
}
