//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::ProgramHead *RecursiveParser<Lexer>::parse_program_head() {

    // program id
    auto program = current_token;
    expected_enum_type(predicate::is_program, predicate::keyword_program);
    auto ident = reinterpret_cast<const Identifier *>(next_token());
    expected_type(TokenType::Identifier);
    next_token();

    for (;;) {
        if (current_token == nullptr || predicate::is_semicolon(current_token)) {
            return new ast::ProgramHead(program, new ast::Ident(ident), nullptr);
        }
        // maybe ( id list )
        if (predicate::is_lparen(current_token)) { break; }
        if (!(predicate::is_begin(current_token) ||
              predicate::is_var(current_token) ||
              predicate::is_procedure(current_token) ||
              predicate::is_function(current_token) ||
              predicate::is_const(current_token))) {
            skip_any_but_eof_token_v(&predicate::marker_lparen)
        }
        errors.push_back(new PascalSParseExpectVGotError(__FUNCTION__, &predicate::marker_lparen, current_token));
        return new ast::ProgramHead(program, new ast::Ident(ident), nullptr);
    }

    // maybe ( id list )
    return new ast::ProgramHead(program, new ast::Ident(ident), parse_id_list_with_paren());
}
