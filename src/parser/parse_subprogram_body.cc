//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::SubprogramBody *RecursiveParser<Lexer>::parse_subprogram_body() {
    // function procedure var const begin, all belong to keyword
    if (!predicate::is_keyword(current_token)) {
        for (;;) {
            if (current_token == nullptr) {
                return fall_expect_s("TokenType::Keyword");
            }
            if (predicate::is_semicolon(current_token)) {
                return fall_expect_s("TokenType::Keyword");
            }
            if (predicate::is_procedure(current_token)) {
                return fall_expect_s("TokenType::Keyword");
            }
            if (predicate::is_function(current_token)) {
                return fall_expect_s("TokenType::Keyword");
            }

            maybe_recover_keyword(KeywordType::Var)
            maybe_recover_keyword(KeywordType::Const)
            maybe_recover_keyword(KeywordType::Begin)

            skip_error_token_s("TokenType::Keyword")
            return fall_expect_s("TokenType::Keyword");
        }
    }

    // const declarations
    ast::ConstDecls *const_decls = nullptr;
    if (predicate::is_const(current_token)) {
        const_decls = parse_const_decls();
    }

    // var declarations
    ast::VarDecls *var_decls = nullptr;
    if (predicate::is_var(current_token)) {
        var_decls = parse_var_decls();
    }

    // compound statement
    return new ast::SubprogramBody(const_decls, var_decls, parse_compound_statement());
}
