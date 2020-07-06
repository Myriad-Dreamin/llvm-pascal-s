//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::SubprogramHead *RecursiveParser<Lexer>::parse_subprogram_head() {

    // program id
    auto fn_def = current_token;
    bool is_fn = predicate::is_function(fn_def);
    next_token();
    assert(fn_def->type == KeywordType::Procedure || fn_def->type == KeywordType::Function);

    auto ident = reinterpret_cast<const Identifier *>(current_token);
    expected_type(TokenType::Identifier);
    next_token();

    for (;;) {

        // may be end of function def
        if (predicate::is_semicolon(current_token)) {

            // function should with type spec
            if (is_fn) {
                errors.push_back(new PascalSParseExpectSGotError(
                        __FUNCTION__, "function param list or function type spec", current_token));
            }
            return new ast::SubprogramHead(fn_def, ident, nullptr, nullptr);
        }

        // maybe param list
        if (predicate::is_lparen(current_token)) { break; }

        // may be function definition
        if (is_fn && predicate::is_colon(current_token)) { break; }

        if (!(predicate::is_begin(current_token) ||
              predicate::is_var(current_token) ||
              predicate::is_const(current_token))) {
            skip_any_but_eof_token_v(&predicate::marker_lparen)
        }
        errors.push_back(new PascalSParseExpectVGotError(__FUNCTION__, &predicate::marker_lparen, current_token));
        return new ast::SubprogramHead(fn_def, ident, nullptr, nullptr);
    }

    // maybe param list
    ast::ParamList *pl = nullptr;
    if (predicate::is_lparen(current_token)) {
        pl = parse_param_list_with_paren();
    }

    // function should with type
    ast::BasicTypeSpec *rt = nullptr;
    if (is_fn) {

        // :
        expected_enum_type_r(predicate::is_colon, predicate::marker_colon, new ast::SubprogramHead(fn_def, ident, pl,
                                                                                                   nullptr));
        next_token();

        if (!predicate::is_keyword(current_token)) {
            for (;;) {
                if (current_token == nullptr) {
                    return fall_expect_s("basic type spec"), new ast::SubprogramHead(fn_def, ident, pl, nullptr);
                }
                if (predicate::is_rparen(current_token)) {
                    return fall_expect_s("basic type spec"), new ast::SubprogramHead(fn_def, ident, pl, nullptr);
                }
                if (predicate::is_semicolon(current_token)) {
                    return fall_expect_s("basic type spec"), new ast::SubprogramHead(fn_def, ident, pl, nullptr);
                }
                maybe_recover_keyword(KeywordType::Boolean)
                maybe_recover_keyword(KeywordType::Integer)
                maybe_recover_keyword(KeywordType::Char)
                maybe_recover_keyword(KeywordType::Real)
                skip_error_token_s("basic type spec")

                return fall_expect_s("basic type spec"), new ast::SubprogramHead(fn_def, ident, pl, nullptr);
            }
        }

        auto basic = current_token;
        rt = new ast::BasicTypeSpec(basic);
        next_token();
    }

    // share common struct type SubprogramHead
    return new ast::SubprogramHead(fn_def, ident, pl, rt);
}

