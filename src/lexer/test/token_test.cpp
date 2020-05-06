
#include <gtest/gtest.h>
#include <pascal-s/lexer.h>

struct TokenTestCase {
    const char *input;
    const char *expected;
};

class GoodTokenTest : public testing::TestWithParam<TokenTestCase> {};

TEST_P(GoodTokenTest, WillNotThrowException) /* NOLINT */
{
    auto &param = GetParam();
    std::stringstream in(param.input);
    FullInMemoryLexer lexer(&in);
    auto tok = lexer.next_token();
    ASSERT_NE(tok, nullptr);
    ASSERT_EQ(tok->type, TokenType::Identifier);
    ASSERT_STREQ(reinterpret_cast<const Identifier *>(tok)->content, param.expected);
}

INSTANTIATE_TEST_SUITE_P(TestIdentifiers, GoodTokenTest, testing::Values( /* NOLINT */
        TokenTestCase{"a {a}", "a"},
        TokenTestCase{"a ", "a"},
        TokenTestCase{"a {a \n a}", "a"}
));
