


#ifndef PASCAL_S_TEST_GTEST_H
#define PASCAL_S_TEST_GTEST_H


#include <pascal-s/token.h>
#include <gtest/gtest.h>
#include <fmt/core.h>

#define ASSERT_TOKEN_EQUAL(val1, val2)\
    ASSERT_PRED_FORMAT2(CompareToken, val1, val2)
//
//ASSERT_EQ(tok->type, TokenType::Identifier);
//ASSERT_STREQ(reinterpret_cast<const Identifier*>(tok)->content, param.expected);


testing::Message &operator<<(testing::Message &msg, const Token *tok) {
    return msg << convertToString(tok);
}

#define equal_field(val1, val2) do{if (val1 != val2) {\
    return internal::CmpHelperEQFailure(lhs_expression, rhs_expression, lhs, rhs);\
}}while(0)

#define str_equal_field(val1, val2) do{if (std::string(val1) != val2) {\
    return internal::CmpHelperEQFailure(lhs_expression, rhs_expression, lhs, rhs);\
}}while(0)

static testing::AssertionResult CompareIdentifier(const char *lhs_expression,
                                                  const char *rhs_expression, const Identifier *lhs,
                                                  const Identifier *rhs) {
    using namespace testing;

    str_equal_field(lhs->content, rhs->content);

    return AssertionSuccess();
}

static testing::AssertionResult CompareComment(const char *lhs_expression,
                                               const char *rhs_expression, const Comment *lhs,
                                               const Comment *rhs) {
    using namespace testing;

    str_equal_field(lhs->content, rhs->content);

    return AssertionSuccess();
}

static testing::AssertionResult CompareConstantChar(const char *lhs_expression,
                                                    const char *rhs_expression, const ConstantChar *lhs,
                                                    const ConstantChar *rhs) {
    using namespace testing;

    equal_field(lhs->attr, rhs->attr);

    return AssertionSuccess();
}

static testing::AssertionResult CompareConstantReal(const char *lhs_expression,
                                                    const char *rhs_expression, const ConstantReal *lhs,
                                                    const ConstantReal *rhs) {
    using namespace testing;

    str_equal_field(lhs->content, rhs->content);
    equal_field(lhs->attr, rhs->attr);

    return AssertionSuccess();
}

static testing::AssertionResult CompareConstantInteger(const char *lhs_expression,
                                                       const char *rhs_expression, const ConstantInteger *lhs,
                                                       const ConstantInteger *rhs) {
    using namespace testing;

    equal_field(lhs->attr, rhs->attr);

    return AssertionSuccess();
}

static testing::AssertionResult CompareConstantBoolean(const char *lhs_expression,
                                                       const char *rhs_expression, const ConstantBoolean *lhs,
                                                       const ConstantBoolean *rhs) {
    using namespace testing;

    equal_field(lhs->attr, rhs->attr);

    return AssertionSuccess();
}


static testing::AssertionResult
CompareMarker(const char *lhs_expression, const char *rhs_expression, const Marker *lhs, const Marker *rhs) {
    using namespace testing;
    str_equal_field(get_marker_type_reversed(lhs->type), get_marker_type_reversed(lhs->type));

    return AssertionSuccess();
}

static testing::AssertionResult
CompareErrorToken(const char *lhs_expression, const char *rhs_expression, const ErrorToken *lhs,
                  const ErrorToken *rhs) {
    using namespace testing;
    str_equal_field(lhs->content, rhs->content);

    return AssertionSuccess();
}


static testing::AssertionResult
CompareKeyword(const char *lhs_expression, const char *rhs_expression, const Keyword *lhs, const Keyword *rhs) {
    using namespace testing;

    str_equal_field(get_keyword_type_reversed(lhs->type), get_keyword_type_reversed(lhs->type));

    return AssertionSuccess();
}

static testing::AssertionResult CompareToken(const char *lhs_expression,
                                             const char *rhs_expression, const Token *lhs,
                                             const Token *rhs) {
    using namespace testing;
    equal_field(lhs->type, rhs->type);
    switch (lhs->type) {
        case TokenType::Identifier:
            return CompareIdentifier(lhs_expression, rhs_expression,
                                     reinterpret_cast<const Identifier *>(lhs),
                                     reinterpret_cast<const Identifier *>(rhs));
        case TokenType::Marker:
            return CompareMarker(lhs_expression, rhs_expression,
                                 reinterpret_cast<const Marker *>(lhs),
                                 reinterpret_cast<const Marker *>(rhs));
        case TokenType::Keyword:
            return CompareKeyword(lhs_expression, rhs_expression,
                                  reinterpret_cast<const Keyword *>(lhs),
                                  reinterpret_cast<const Keyword *>(rhs));
        case TokenType::ConstantChar:
            return CompareConstantChar(lhs_expression, rhs_expression,
                                       reinterpret_cast<const ConstantChar *>(lhs),
                                       reinterpret_cast<const ConstantChar *>(rhs));
        case TokenType::ConstantReal:
            return CompareConstantReal(lhs_expression, rhs_expression,
                                       reinterpret_cast<const ConstantReal *>(lhs),
                                       reinterpret_cast<const ConstantReal *>(rhs));
        case TokenType::ConstantInteger:
            return CompareConstantInteger(lhs_expression, rhs_expression,
                                          reinterpret_cast<const ConstantInteger *>(lhs),
                                          reinterpret_cast<const ConstantInteger *>(rhs));
        case TokenType::ConstantBoolean:
            return CompareConstantBoolean(lhs_expression, rhs_expression,
                                          reinterpret_cast<const ConstantBoolean *>(lhs),
                                          reinterpret_cast<const ConstantBoolean *>(rhs));
        case TokenType::ErrorToken:
            return CompareErrorToken(lhs_expression, rhs_expression,
                                     reinterpret_cast<const ErrorToken *>(lhs),
                                     reinterpret_cast<const ErrorToken *>(rhs));
        case TokenType::Comment:
            return CompareComment(lhs_expression, rhs_expression,
                                  reinterpret_cast<const Comment *>(lhs),
                                  reinterpret_cast<const Comment *>(rhs));
        default:
            return AssertionFailure() << fmt::format("unknown token type {}", lhs->type);
    }
    assert(false);
}


#undef equal_field
#undef str_equal_field

#endif

