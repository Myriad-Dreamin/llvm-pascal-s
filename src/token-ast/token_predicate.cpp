//
// Created by kamiyoru on 2020/6/26.
//

#include <pascal-s/token.h>
#include <vector>
#include <set>
#include <stdexcept>


namespace predicate {
#define marker_predicator(lower, upper) bool is_ ## lower(const Token *tok) {\
    return tok != nullptr && tok-> type == MarkerType::upper; \
}\
const Marker marker_## lower(MarkerType::upper);
#define keyword_predicator(lower, upper) bool is_ ## lower(const Token *tok) {\
    return tok != nullptr && tok-> type == KeywordType::upper; \
}\
const Keyword keyword_## lower(KeywordType::upper);

    marker_predicator(logic_not, LogicNot)

    marker_predicator(logic_and, LogicAnd)

    marker_predicator(logic_or, LogicOr)

    marker_predicator(neq, NEQ)

    marker_predicator(le, LE)

    marker_predicator(ge, GE)

    marker_predicator(lt, LT)

    marker_predicator(eq, EQ)

    marker_predicator(gt, GT)

    marker_predicator(range, Range)

    marker_predicator(assgin, Assign)

    marker_predicator(add, Add)

    marker_predicator(sub, Sub)

    marker_predicator(mul, Mul)

    marker_predicator(div, Div)

//    marker_predicator(mod, Mod)

    marker_predicator(lparen, LParen)

    marker_predicator(rparen, RParen)

    marker_predicator(lbracket, LBracket)

    marker_predicator(rbracket, RBracket)

    marker_predicator(comma, Comma)

    marker_predicator(dot, Dot)

    marker_predicator(semicolon, Semicolon)

    marker_predicator(colon, Colon)

    keyword_predicator(program, Program)

    keyword_predicator(const, Const)

    keyword_predicator(var, Var)

    keyword_predicator(procedure, Procedure)

    keyword_predicator(function, Function)

    keyword_predicator(begin, Begin)

    keyword_predicator(end, End)

    keyword_predicator(array, Array)

    keyword_predicator(integer, Integer)

    keyword_predicator(real, Real)

    keyword_predicator(boolean, Boolean)

    keyword_predicator(char, Char)

    keyword_predicator(if, If)

    keyword_predicator(then, Then)

    keyword_predicator(else, Else)

    keyword_predicator(for, For)

    keyword_predicator(to, To)

    keyword_predicator(do, Do)

    keyword_predicator(of, Of)

    keyword_predicator(read, Read)

    keyword_predicator(write, Write)

    [[maybe_unused]] bool token_equal(const Token *lhs, const std::vector<Token *> *rhs) {
        if (rhs == nullptr || lhs == nullptr) {
            return lhs == nullptr;
        }

        for (auto r: *rhs) {
            if (token_equal(lhs, r)) {
                return true;
            }
        }
        return false;
    }

    bool token_equal(const Token *lhs, const std::set<const Token *> *rhs) {
        if (rhs == nullptr || lhs == nullptr) {
            return lhs == nullptr;
        }

        for (auto r: *rhs) {
            if (token_equal(lhs, r)) {
                return true;
            }
        }
        return false;
    }

    bool token_equal(const Token *lhs, const Token *rhs) {
        if (lhs == nullptr && rhs == nullptr) {
            return true;
        }
        if (lhs == nullptr || rhs == nullptr) {
            return false;
        }
        return !(lhs->type != rhs->type);

    }

    bool is_const_token(TokenType tt) {
        return TokenType::ConstRangeL <= tt && tt <= TokenType::ConstRangeR;
    }

    bool is_binary_sign(const Token *t) {
        return t != nullptr && is_binary_sign(reinterpret_cast<const Marker *>(t)->type);
    }

    bool is_binary_sign(MarkerType mt) {
        return MarkerType::LogicNot <= mt && mt <= MarkerType::LogicAnd;
    }

    bool is_marker(const Token *t) {
        return t != nullptr && is_marker_type(t->type);
    }

    bool is_marker_type(MarkerType t) {
        return TokenType::MarkerRangeL <= t && t < TokenType::MarkerRangeR;
    }

    bool is_keyword(const Token *t) {
        return t != nullptr && is_keyword_type(t->type);
    }

    bool is_keyword_type(KeywordType t) {
        return TokenType::KeywordRangeL <= t && t < TokenType::KeywordRangeR;
    }

#undef marker_predicator
#undef keyword_predicator
}