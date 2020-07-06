//
// Created by kamiyoru on 2020/6/27.
//


#include <pascal-s/parser.h>
#include <pascal-s/parser_recursive.h>

template<typename Lexer>
ast::Exp *RecursiveParser<Lexer>::parse_const_exp(const std::set<const Token *> *till) {

    auto *maybe_lhs = parse_const_fac(till);
    if (maybe_lhs == nullptr) {
        return nullptr;
    }

    return maybe_lhs;
}

template<typename Lexer>
ast::Exp *RecursiveParser<Lexer>::parse_const_fac(const std::set<const Token *> *till) {
    if (current_token == nullptr) {
        return fall_expect_s("const value");
    }

    if (!predicate::is_const_token(current_token->type)) {
        for (;;) {
            if (predicate::token_equal(current_token, till)) {
                return fall_expect_s("constant fac");
            }
            if (predicate::is_add(current_token)) {
                break;
            }
            if (predicate::is_sub(current_token)) {
                break;
            }
            if (predicate::is_lparen(current_token)) {
                break;
            }
            if (predicate::is_const_token(current_token->type)) {
                break;
            }
            skip_any_but_eof_token_s("constant fac")
            return fall_expect_s("constant fac");
        }
    }

    if (predicate::is_marker_type(current_token->type)) {
        auto marker = current_token;
        switch (marker->type) {
            case MarkerType::Add:
            case MarkerType::Sub:
                next_token();
                return new ast::UnExp(marker, parse_const_exp(till));
            case MarkerType::LParen:
                next_token();
                return parse_const_exp(&predicate::predicateContainers.rParenContainer);
            default:
                assert(false);
                throw std::runtime_error("expected +/-/(");
        }
    }

    // parse const fac
    ast::Exp *fac = nullptr;

    switch (current_token->type) {

        case TokenType::ConstantInteger:
            fac = new ast::ExpConstantInteger(
                    reinterpret_cast<const ConstantInteger *>(current_token));
            break;
        case TokenType::ConstantChar:
            fac = new ast::ExpConstantChar(
                    reinterpret_cast<const ConstantChar *>(current_token));
            break;
        case TokenType::ConstantString:
            fac = new ast::ExpConstantString(
                    reinterpret_cast<const ConstantString *>(current_token));
            break;
        case TokenType::ConstantReal:
            fac = new ast::ExpConstantReal(
                    reinterpret_cast<const ConstantReal *>(current_token));
            break;
        case TokenType::ConstantBoolean:
            fac = new ast::ExpConstantBoolean(
                    reinterpret_cast<const ConstantBoolean *>(current_token));
            break;
        default:
            assert(false);
            throw std::runtime_error("expect constant fac");
    }
    next_token();
    return fac;
}
