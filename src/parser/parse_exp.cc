//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::Exp *RecursiveParser<Lexer>::parse_exp(const std::set<const Token *> *till) {

    // lhs
    auto maybe_lhs = parse_fac(till);

    for (;;) {
        if (predicate::token_equal(current_token, till)) {
            return maybe_lhs;
        }
        if (predicate::is_rparen(current_token)) {
            break;
        }
        if (predicate::is_binary_sign(current_token)) {
            break;
        }
        if (predicate::is_dot(current_token)) {
            break;
        }
        if (predicate::is_assgin(current_token)) {
            break;
        }
        skip_any_but_eof_token_s("expression fac")
        return fall_expect_s("expression fac");
    }

    if (predicate::is_marker(current_token)) {
        auto marker = current_token;
        switch (marker->type) {

            // fix follow markers
            case MarkerType::Dot:
            case MarkerType::RParen:
                return maybe_lhs;

                // a + b
            case MarkerType::Add:
                // a - b
            case MarkerType::Sub:
                // a * b
            case MarkerType::Mul:
                // a / b
            case MarkerType::Div:
                // a % b
            case MarkerType::Mod:
                // a and b
            case MarkerType::LogicAnd:
                // a or b
            case MarkerType::LogicOr:
                // a < b
            case MarkerType::LT:
                // a <= b
            case MarkerType::LE:
                // a > b
            case MarkerType::GT:
                // a >= b
            case MarkerType::GE:
                // a = b
            case MarkerType::EQ:
                // a <> b
            case MarkerType::NEQ:
                next_token();
                return parse_binary_exp(maybe_lhs, marker, get_marker_pri(marker->type), till);

                // a := b
            case MarkerType::Assign:
                next_token();
                return new ast::ExpAssign(maybe_lhs, parse_exp(till));
            default:
                assert(false);
                return nullptr;
        }
    } else {
        assert(false);
        throw std::runtime_error(
                std::string("want marker, got ") + fmt::format("{}",
                                                               convertToString(current_token))
        );
    }

}

template<typename Lexer>
ast::Exp *RecursiveParser<Lexer>::parse_fac(const std::set<const Token *> *till) {
    if (current_token == nullptr) {
        return fall_expect_s("expression fac");
    }

    for (;;) {
        if (predicate::token_equal(current_token, till)) {
            return fall_expect_s("expression fac");
        }
        if (predicate::is_add(current_token)) {
            break;
        }
        if (predicate::is_sub(current_token)) {
            break;
        }
        if (predicate::is_logic_not(current_token)) {
            break;
        }
        if (predicate::is_lparen(current_token)) {
            break;
        }
        if (predicate::is_const_token(current_token->type)) {
            break;
        }
        if (current_token->type == TokenType::Identifier) {
            break;
        }
        skip_any_but_eof_token_s("expression fac")
        return fall_expect_s("expression fac");
    }

    // const factors
    ast::Exp *e = nullptr;
    switch (current_token->type) {
        case TokenType::ConstantInteger:
            e = new ast::ExpConstantInteger(
                    reinterpret_cast<const ConstantInteger *>(current_token));
            break;
        case TokenType::ConstantChar:
            e = new ast::ExpConstantChar(
                    reinterpret_cast<const ConstantChar *>(current_token));
            break;
        case TokenType::ConstantReal:
            e = new ast::ExpConstantReal(
                    reinterpret_cast<const ConstantReal *>(current_token));
            break;
        case TokenType::ConstantString:
            e = new ast::ExpConstantString(
                    reinterpret_cast<const ConstantString *>(current_token));
            break;
        case TokenType::ConstantBoolean:
            e = new ast::ExpConstantBoolean(
                    reinterpret_cast<const ConstantBoolean *>(current_token));
            break;
    }

    if (e != nullptr) {
        next_token();
        return e;
    }


    // unary expression or paren
    if (predicate::is_marker(current_token)) {
        auto marker = current_token;
        switch (marker->type) {
            default:
                assert(false);
                throw std::runtime_error("expected not/+/-/(");

                // not exp
            case MarkerType::LogicNot:

                // + exp
            case MarkerType::Add:

                // - exp
            case MarkerType::Sub:
                next_token();
                return new ast::UnExp(marker, parse_exp(till));

                // (  )
            case MarkerType::LParen:
                next_token();

                auto exp = parse_exp(&predicate::predicateContainers.rParenContainer);
                if (exp != nullptr) {
                    expected_enum_type_r(predicate::is_rparen, predicate::marker_rparen, exp);
                    next_token();
                }
                return exp;
        }

        // function call or array index or identifier exp
        // ident
    } else if (current_token->type == TokenType::Identifier) {
        auto ident = reinterpret_cast<const Identifier *>(current_token);
        next_token();

        for (;;) {
            skip_error_token_s("expression fac or [/(")
            break;
        }

        // ( or [
        if (predicate::is_marker(current_token)) {
            auto marker = current_token;
            if (marker->type == MarkerType::LParen) {

                // will not eat (, just parse ( expression list )
                return new ast::ExpCall(
                        ident, parse_expression_list_with_paren());
                // [
            } else if (marker->type == MarkerType::LBracket) {
                auto variable = new ast::Variable();
                variable->id = ident;
                // will not eat [, just parse [ expression list ]
                variable->id_var = parse_expression_list_with_bracket();
                ast::copy_pos_between_tokens(variable, ident, variable->id_var);
                return variable;
            }
        }

        // otherwise just a identifier exp
        return new ast::Ident(ident);
    }

    assert(false);
    throw std::runtime_error("expected expression fac");
}

template<typename Lexer>
ast::Exp *
RecursiveParser<Lexer>::parse_binary_exp(ast::Exp *lhs, const Marker *marker,
                                         pascal_s::marker_type_underlying_type current_marker_pri,
                                         const std::set<const Token *> *till) {

    auto rhs = parse_fac(till);

    for (;;) {
        if (predicate::token_equal(current_token, till)) {
            return new ast::BiExp(lhs, marker, rhs);
        }
        if (predicate::is_binary_sign(current_token)) {
            break;
        }
        skip_error_token_s("TokenType::Marker");
        return fall_expect_s("TokenType::Marker"), new ast::BiExp(lhs, marker, rhs);
    }

    auto pri = get_marker_pri(current_token->type);
    auto next_marker = current_token;
    next_token();
    if (current_marker_pri >= pri) {
        return parse_binary_exp(new ast::BiExp(lhs, marker, rhs),
                                next_marker, pri, till);
    }
    return new ast::BiExp(lhs, marker, parse_binary_exp(rhs, next_marker, pri, till));
}
