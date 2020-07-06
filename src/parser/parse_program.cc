//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::Program *RecursiveParser<Lexer>::parse_program() {

    // program head
    auto ph = parse_program_head();
    if (ph == nullptr) {
        return nullptr;
    }

    // ;
    expected_enum_type_r(predicate::is_semicolon, predicate::marker_semicolon,
                         new ast::Program(ph, nullptr));
    auto semicolon = current_token;
    next_token();

    // program body
    auto program = new ast::Program(ph, parse_program_body());
    program->semicolon = semicolon;

//    // .
//    expected_enum_type_r(predicate::is_dot, predicate::marker_dot, program);
//    program->dot = current_token;
//    next_token();

    return program;
}
