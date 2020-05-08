
#include <gtest/gtest.h>
#include <pascal-s/token.h>
#include <pascal-s/mock.h>
#include <pascal-s/parser.h>

// ParserGetNextToken Test
// 测试 next_token


struct ParserTestCase {

    std::vector<Token *> token_stream;
//    ASTNode *expected = nullptr;
};

struct ParserTest : public testing::TestWithParam<ParserTestCase> {

    void TearDown() override {
        for (auto tok : GetParam().token_stream) deleteToken(tok);
    };
};

struct ParserMainTest : public ParserTest {
};

TEST_P(ParserMainTest, WillNotThrowException) /* NOLINT */
{
    auto &&param = GetParam();
    MockLexer lexer(param.token_stream);
    LexerProxy<MockLexer> lexer_proxy(lexer);
    Parser parser(lexer_proxy);
    auto ast = parser.parse();
    ASSERT_NE(ast, nullptr);

    deleteAST(ast);
}

INSTANTIATE_TEST_SUITE_P(Simple, ParserMainTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::LParen),
                        new Marker(MarkerType::RParen),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::LParen),
                        new Identifier("a"),
                        new Marker(MarkerType::RParen),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::LParen),
                        new Identifier("a"),
                        new Marker(MarkerType::Comma),
                        new Identifier("b"),
                        new Marker(MarkerType::RParen),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger("1"),
                        new Marker(MarkerType::Semicolon),
                }
        }
));



struct ParserConstDeclsTest : public ParserTest {
};

TEST_P(ParserConstDeclsTest, WillNotThrowException) /* NOLINT */
{
    auto &&param = GetParam();
    MockLexer lexer(param.token_stream);
    LexerProxy<MockLexer> lexer_proxy(lexer);
    Parser parser(lexer_proxy);
    parser.next_token();
    auto ast = parser.parse_const_decls();
    ASSERT_NE(ast, nullptr);

    deleteAST(ast);
//        auto tok = lexer.next_token();
//        ASSERT_NE(tok, nullptr);
//        ASSERT_TOKEN_EQUAL(tok, param.expected);
}

INSTANTIATE_TEST_SUITE_P(Simple, ParserConstDeclsTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger("1"),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger("1"),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("b"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger("2"),
                        new Marker(MarkerType::Semicolon),
                }
        }
));

