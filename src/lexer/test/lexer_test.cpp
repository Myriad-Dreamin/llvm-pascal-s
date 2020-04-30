
#include "token_helper.h"
#include <gtest/gtest.h>
#include <pascal-s/lexer.h>
#include <pascal-s/token.h>

struct LexerGetNextTokenTestCase {
    const char *input;
    Token *expected = nullptr;
};

class GoodLexerGetNextTokenTest : public testing::TestWithParam<LexerGetNextTokenTestCase> {

    void TearDown() override {
        deleteToken(GetParam().expected);
    };
};

TEST_P(GoodLexerGetNextTokenTest, WillNotThrowException) /* NOLINT */
{
    auto &&param = GetParam();
    std::stringstream in(param.input);
    FullInMemoryLexer lexer(&in);
    auto tok = lexer.next_token();
    ASSERT_NE(tok, nullptr);
    ASSERT_TOKEN_EQUAL(tok, param.expected);
}

INSTANTIATE_TEST_SUITE_P(TestIdentifiers, GoodLexerGetNextTokenTest, testing::Values( /* NOLINT */
        LexerGetNextTokenTestCase{"a", new Identifier("a")},
        LexerGetNextTokenTestCase{"a ", new Identifier("a")}
));
