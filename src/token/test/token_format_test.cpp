
#include <gtest/gtest.h>
#include <pascal-s/token.h>

struct TokenFormatTestCase {
    Token *tok;
    const char *expected;
};

class GoodTokenFormatTest : public testing::TestWithParam<TokenFormatTestCase> {

    void TearDown() override {
        auto &tok = GetParam().tok;
        if (tok != nullptr) {
            deleteToken(tok);
        }
    };
};

TEST_P(GoodTokenFormatTest, WillNotThrowException) /* NOLINT */
{
    auto &param = GetParam();
    ASSERT_STREQ(param.expected, convertToString(param.tok).c_str());
}

INSTANTIATE_TEST_SUITE_P(TestIdentifiers, GoodTokenFormatTest, testing::Values( /* NOLINT */
        TokenFormatTestCase{new Identifier("a"), "{ .type = Identifier .content = a }"}
));
