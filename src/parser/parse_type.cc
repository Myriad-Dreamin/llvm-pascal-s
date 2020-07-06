//
// Created by kamiyoru on 2020/6/27.
//
//maybe_fall_eof

#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::TypeSpec *RecursiveParser<Lexer>::parse_type() {
    // next token == array | basic_type, all belong to keyword
    if (!predicate::is_keyword(current_token)) {
        for (;;) {
            if (current_token == nullptr) {
                return fall_expect_s("TokenType::Keyword");
            }
            if (predicate::is_rparen(current_token)) {
                return fall_expect_s("TokenType::Keyword");
            }
            if (predicate::is_semicolon(current_token)) {
                return fall_expect_s("TokenType::Keyword");
            }
            if (predicate::is_end(current_token)) {
                return fall_expect_s("TokenType::Keyword");
            }
            maybe_recover_keyword(KeywordType::Array)
            maybe_recover_keyword(KeywordType::Boolean)
            maybe_recover_keyword(KeywordType::Integer)
            maybe_recover_keyword(KeywordType::Char)
            maybe_recover_keyword(KeywordType::Real)
            skip_error_token_s("TokenType::Keyword")
            return fall_expect_s("TokenType::Keyword");
        }
    }

    auto array_or_basic = current_token;

    // array
    if (array_or_basic->type == KeywordType::Array) {
        next_token();
        return parse_array_type(array_or_basic);
    } else if (
        // basic type
            array_or_basic->type == KeywordType::Integer ||
            array_or_basic->type == KeywordType::Real ||
            array_or_basic->type == KeywordType::Char ||
            array_or_basic->type == KeywordType::Boolean
            ) {
        next_token();
        return new ast::BasicTypeSpec(array_or_basic);
    }
    errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", array_or_basic));
    return nullptr;
}

#define ps_bad_reduced_array (ast::copy_pos_with_check(arr_type_spec, keyword_array), arr_type_spec)

template<typename Lexer>
ast::ArrayTypeSpec *RecursiveParser<Lexer>::parse_array_type(const Keyword *keyword_array) {
    auto arr_type_spec = new ast::ArrayTypeSpec(keyword_array);

    // [
    expected_enum_type_r(predicate::is_lbracket, predicate::marker_lbracket, ps_bad_reduced_array);
    next_token();


    // digit
    for (;;) {

        // digit
        if (current_token == nullptr || current_token->type != TokenType::ConstantInteger) {
            for (;;) {
                if (current_token == nullptr) {
                    return fall_expect_t(TokenType::ConstantInteger), ps_bad_reduced_array;
                }
                if (predicate::is_semicolon(current_token)) {
                    return fall_expect_t(TokenType::ConstantInteger), ps_bad_reduced_array;
                }
                if (predicate::is_end(current_token)) {
                    return fall_expect_t(TokenType::ConstantInteger), ps_bad_reduced_array;
                }
                if (current_token->type == TokenType::ConstantInteger) {
                    break;
                }
                if (current_token->type == TokenType::ConstantReal) {
                    errors.push_back(
                            new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                    update_guess(new ConstantInteger(static_cast<pascal_s::pascal_s_integer_t>(
                                                             reinterpret_cast<const ConstantReal *>(current_token)->attr)));
                    break;
                }
                if (current_token->type == TokenType::Identifier) {
                    errors.push_back(
                            new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                    update_guess(new ConstantInteger(0));
                    break;
                }
                skip_error_token_t(TokenType::ConstantInteger)
                errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "left_range", current_token));
                return ps_bad_reduced_array;
            }
        }


        auto l_range = reinterpret_cast<const ConstantInteger *>(current_token);
        next_token();

        // ..
        expected_enum_type_r(predicate::is_range, predicate::marker_range, ps_bad_reduced_array);
        next_token();

        // digit
        if (current_token == nullptr || current_token->type != TokenType::ConstantInteger) {
            for (;;) {
                if (current_token == nullptr) {
                    return fall_expect_t(TokenType::ConstantInteger), ps_bad_reduced_array;
                }
                if (predicate::is_add(current_token)) {
                    errors.push_back(
                            new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                    next_token();
                    continue;
                }
                if (predicate::is_sub(current_token)) {
                    errors.push_back(
                            new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                    next_token();
                    continue;
                }
                if (predicate::is_semicolon(current_token)) {
                    return fall_expect_t(TokenType::ConstantInteger), ps_bad_reduced_array;
                }
                if (predicate::is_end(current_token)) {
                    return fall_expect_t(TokenType::ConstantInteger), ps_bad_reduced_array;
                }
                if (current_token->type == TokenType::ConstantInteger) {
                    break;
                }
                if (current_token->type == TokenType::ConstantReal) {
                    errors.push_back(
                            new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                    update_guess(new ConstantInteger(static_cast<pascal_s::pascal_s_integer_t>(
                                                             reinterpret_cast<const ConstantReal *>(current_token)->attr)));
                    break;
                }
                if (current_token->type == TokenType::Identifier) {
                    errors.push_back(
                            new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                    update_guess(new ConstantInteger(0));
                    break;
                }
                skip_error_token_t(TokenType::ConstantInteger)
                errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "right_range", current_token));
                return ps_bad_reduced_array;
            }
        }
        arr_type_spec->periods.emplace_back(
                int64_t(l_range->attr),
                int64_t(reinterpret_cast<const ConstantInteger *>(current_token)->attr));
        next_token();

        // ,
        if (!predicate::is_comma(current_token)) {
            break;
        }
        next_token();
    }

    // ]
    expected_enum_type_r(predicate::is_rbracket, predicate::marker_rbracket, arr_type_spec);
    next_token();

    // of
    expected_enum_type_r(predicate::is_of, predicate::keyword_of, arr_type_spec);
    next_token();

    if (!predicate::is_keyword(current_token)) {
        for (;;) {
            if (current_token == nullptr) {
                return fall_expect_s("TokenType::Keyword"), arr_type_spec;
            }
            if (predicate::is_add(current_token)) {
                errors.push_back(
                        new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                next_token();
                continue;
            }
            if (predicate::is_sub(current_token)) {
                errors.push_back(
                        new PascalSParseExpectTGotError(__FUNCTION__, TokenType::ConstantInteger, current_token));
                next_token();
                continue;
            }
            if (predicate::is_rparen(current_token)) {
                return fall_expect_s("TokenType::Keyword"), arr_type_spec;
            }
            if (predicate::is_semicolon(current_token)) {
                return fall_expect_s("TokenType::Keyword"), arr_type_spec;
            }
            if (predicate::is_end(current_token)) {
                return fall_expect_s("TokenType::Keyword"), arr_type_spec;
            }
            maybe_recover_keyword(KeywordType::Boolean)
            maybe_recover_keyword(KeywordType::Integer)
            maybe_recover_keyword(KeywordType::Char)
            maybe_recover_keyword(KeywordType::Real)
            skip_error_token_s("TokenType::Keyword")
            return fall_expect_s("TokenType::Keyword"), arr_type_spec;
        }
    }

    // basic_type
    auto basic = current_token;
    if (
            basic->type == KeywordType::Integer ||
            basic->type == KeywordType::Real ||
            basic->type == KeywordType::Char ||
            basic->type == KeywordType::Boolean
            ) {
        next_token();
        arr_type_spec->keyword = basic;
        ast::copy_pos_between_tokens(arr_type_spec, keyword_array, basic);
        return arr_type_spec;
    }

    errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", basic));
    return nullptr;
}

#undef ps_bad_reduced_array