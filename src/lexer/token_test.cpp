
#include <gtest/gtest.h>
#include "lexer.h"

struct TokenTestCase {
    const char *input;
    const char *expected;
};

class GoodTokenTest : public testing::TestWithParam<TokenTestCase> {};

TEST_P(GoodTokenTest, WillNotThrowException) /* NOLINT */
{
    auto &param = GetParam();
    std::stringstream in(param.input);
    Lexer lexer(&in);

    ASSERT_NE(lexer.yylex(), 0);
    ASSERT_STREQ(lexer.latest, param.expected);
}

INSTANTIATE_TEST_SUITE_P(TestIdentifiers, GoodTokenTest, testing::Values( /* NOLINT */
        TokenTestCase{"a", "a"},
        TokenTestCase{"a ", "a"}
));
