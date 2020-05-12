
#include <gtest/gtest.h>
#include <pascal-s/token.h>
#include <pascal-s/lexer.h>
#include "token_helper.h"

// LexerGetNextToken Test
// 测试 next_token


struct LexerGetNextTokenTestCase {

    const char *input = nullptr;
    Token *expected = nullptr;
};

template<typename TestingLexer>
struct GoodLexerGetNextTokenTest : public testing::TestWithParam<LexerGetNextTokenTestCase> {
    using Lexer = TestingLexer;

    void TearDown() override {
        deleteToken(GetParam().expected);
    };

    static auto TestMain() {
        auto &&param = GetParam();
        std::stringstream in(param.input);
        Lexer lexer(&in);
        auto tok = lexer.next_token();
        ASSERT_NE(tok, nullptr);
        ASSERT_TOKEN_EQUAL(tok, param.expected);
    }

    [[maybe_unused]] static auto TestValues() {
        return testing::Values(
                LexerGetNextTokenTestCase{"a", new Identifier("a")},
                LexerGetNextTokenTestCase{"a ", new Identifier("a")}
        );
    }
};

#define GetNextTokenTest(LexerName) using Good##LexerName##GetNextTokenTest = GoodLexerGetNextTokenTest<LexerName>;\
    TEST_P(Good##LexerName##GetNextTokenTest, WillNotThrowException) /* NOLINT */\
        {Good##LexerName##GetNextTokenTest::TestMain();}\
    INSTANTIATE_TEST_SUITE_P(TestIdentifiers, Good##LexerName##GetNextTokenTest,\
        Good##LexerName##GetNextTokenTest::TestValues()) /* NOLINT */

GetNextTokenTest(FullInMemoryLexer);


// LexerGetNextToken Test
// 测试 next_token

struct LexerGetAllTokensTestCase {
    const char *input;
    std::vector<Token *> *expected;
};

class GoodLexerGetAllTokensTest : public testing::TestWithParam<LexerGetAllTokensTestCase> {

    void TearDown() override {
        auto &tokens = GetParam().expected;
        for (auto tok: *tokens) {
            deleteToken(tok);
        }
        tokens->clear();
        delete tokens;
    };
};

TEST_P(GoodLexerGetAllTokensTest, WillNotThrowException) /* NOLINT */
{
    auto &&param = GetParam();
    std::stringstream in(param.input);
    FullInMemoryLexer lexer(&in);
    auto tokens = lexer.get_all_tokens();
    ASSERT_EQ(tokens.size(), param.expected->size());
    for (size_t i = 0; i < tokens.size(); i++) {
        ASSERT_TOKEN_EQUAL(tokens[i], (*param.expected)[i]);
    }
}

INSTANTIATE_TEST_SUITE_P(Simple, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        LexerGetAllTokensTestCase{"a",
                                  new std::vector<Token *>({new Identifier("a")})},
        LexerGetAllTokensTestCase{"mod",
                                  new std::vector<Token *>({new Marker(MarkerType::Mod)})},
        LexerGetAllTokensTestCase{"a ",
                                  new std::vector<Token *>({new Identifier("a")})},
        LexerGetAllTokensTestCase{"a b",
                                  new std::vector<Token *>({new Identifier("a"), new Identifier("b")})},
        LexerGetAllTokensTestCase{"a b\n a",
                                  new std::vector<Token *>({
                                                                   new Identifier("a"), new Identifier("b"),
                                                                   new Identifier("a")})}
));

