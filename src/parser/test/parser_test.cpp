
#include "parser_test_helper.h"

struct ParserMainTest : public ParserTest {
};

PARSER_BASIC_TEST_P(ParserMainTest, parse)

INSTANTIATE_TEST_SUITE_P(Simple, ParserMainTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
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
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
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
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
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
                        new ConstantInteger(1),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Var),
                        new Identifier("a"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Var),
                        new Identifier("a"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Procedure),
                        new Identifier("main"),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger(1),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Begin),
                        new Keyword(KeywordType::End),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
                }
        }
));

INSTANTIATE_TEST_SUITE_P(HelloWorld, ParserMainTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Begin),
                        new Identifier("write_char"),
                        new Marker(MarkerType::LParen),
                        new ConstantChar('h'),
                        new Marker(MarkerType::RParen),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("main"),
                        new Marker(MarkerType::Assign),
                        new ConstantInteger(0),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::End),
                }
        }
));

//struct ParserConstDeclsTest : public ParserTest {
//};
//
//TEST_P(ParserConstDeclsTest, WillNotThrowException) /* NOLINT */
//{
//    auto &&param = GetParam();
//    MockLexer lexer(param.token_stream);
//    LexerProxy <MockLexer> lexer_proxy(lexer);
//    Parser parser(lexer_proxy);
//    parser.next_token();
//    auto ast = parser.parse_const_decls();
//    ASSERT_NE(ast, nullptr);
//
//    deleteAST(ast);
//}
//
//INSTANTIATE_TEST_SUITE_P(Simple, ParserConstDeclsTest, testing::Values( /* NOLINT */
//        ParserTestCase{
//                {
//                        new Keyword(KeywordType::Const),
//                        new Identifier("a"),
//                        new Marker(MarkerType::EQ),
//                        new ConstantInteger("1"),
//                        new Marker(MarkerType::Semicolon),
//                }
//        },
//        ParserTestCase{
//                {
//                        new Keyword(KeywordType::Const),
//                        new Identifier("a"),
//                        new Marker(MarkerType::EQ),
//                        new ConstantInteger("1"),
//                        new Marker(MarkerType::Semicolon),
//                        new Identifier("b"),
//                        new Marker(MarkerType::EQ),
//                        new ConstantInteger("2"),
//                        new Marker(MarkerType::Semicolon),
//                }
//        }
//));
//
//
//struct ParserVarDeclsTest : public ParserTest {
//};
//
//TEST_P(ParserVarDeclsTest, WillNotThrowException) /* NOLINT */
//{
//    auto &&param = GetParam();
//    MockLexer lexer(param.token_stream);
//    LexerProxy <MockLexer> lexer_proxy(lexer);
//    Parser parser(lexer_proxy);
//    parser.next_token();
//    auto ast = parser.parse_var_decls();
//    ASSERT_NE(ast, nullptr);
//
//    deleteAST(ast);
//}
//
//INSTANTIATE_TEST_SUITE_P(Simple, ParserVarDeclsTest, testing::Values( /* NOLINT */
//        ParserTestCase{
//                {
//                        new Keyword(KeywordType::Var),
//                        new Identifier("a"),
//                        new Marker(MarkerType::Colon),
//                        new Keyword(KeywordType::Boolean),
//                }
//        },
//        ParserTestCase{
//                {
//                        new Keyword(KeywordType::Var),
//                        new Identifier("a"),
//                        new Marker(MarkerType::Comma),
//                        new Identifier("b"),
//                        new Marker(MarkerType::Colon),
//                        new Keyword(KeywordType::Boolean),
//                }
//        },
//        ParserTestCase{
//                {
//                        new Keyword(KeywordType::Var),
//                        new Identifier("a"),
//                        new Marker(MarkerType::Colon),
//                        new Keyword(KeywordType::Array),
//                        new Marker(MarkerType::LBracket),
//                        new Marker(MarkerType::RBracket),
//                        new Keyword(KeywordType::Of),
//                        new Keyword(KeywordType::Boolean),
//                }
//        }
//));
//
//
//struct ParserExpTest : public ParserTest {
//};
//
//TEST_P(ParserExpTest, WillNotThrowException) /* NOLINT */
//{
//    auto &&param = GetParam();
//    MockLexer lexer(param.token_stream);
//    LexerProxy <MockLexer> lexer_proxy(lexer);
//    Parser parser(lexer_proxy);
//    parser.next_token();
//    auto ast = parser.parse_exp();
//    ASSERT_NE(ast, nullptr);
//    auto x = lexer.current_token_cursor;
//    parser.next_token();
//    ASSERT_EQ(x, lexer.current_token_cursor);
//
//    deleteAST(ast);
////        auto tok = lexer.next_token();
////        ASSERT_NE(tok, nullptr);
////        ASSERT_TOKEN_EQUAL(tok, param.expected);
//}
//
//INSTANTIATE_TEST_SUITE_P(Simple, ParserExpTest, testing::Values( /* NOLINT */
//        ParserTestCase{
//                {
//                        new ConstantInteger("1"),
//                }
//        },
//        ParserTestCase{
//                {
//                        new ConstantInteger("1"),
//                        new Marker(MarkerType::Add),
//                        new ConstantInteger("1"),
//                }
//        },
//        ParserTestCase{
//                {
//                        new ConstantInteger("1"),
//                        new Marker(MarkerType::Add),
//                        new Marker(MarkerType::LParen),
//                        new ConstantInteger("1"),
//                        new Marker(MarkerType::Add),
//                        new ConstantInteger("1"),
//                        new Marker(MarkerType::RParen),
//                }
//        }
//));
