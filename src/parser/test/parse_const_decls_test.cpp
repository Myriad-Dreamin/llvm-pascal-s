
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct ConstDeclsTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(ConstDeclsTest, parse_const_decls)


INSTANTIATE_TEST_SUITE_P(Simple, ConstDeclsTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger(1),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger(1),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger(1),
                        new Marker(MarkerType::Semicolon),
                }
        }
));


struct GuessConstDeclsTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessConstDeclsTest, parse_const_decls)


INSTANTIATE_TEST_SUITE_P(Simple, GuessConstDeclsTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger(1),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger(1),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("a"),
                }
        }
));

struct GuessStopConstDeclsTest : public ParserTest {
};

PARSER_GUESS_STOP_SUB_BASIC_TEST_P(GuessStopConstDeclsTest, parse_const_decls)


INSTANTIATE_TEST_SUITE_P(Simple, GuessStopConstDeclsTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Var),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Function),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Procedure),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Begin),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Var),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Identifier("a"),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Var),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Identifier("a"),
                        new ErrorToken("error", raw_length_of("error")),
                        new Marker(MarkerType::EQ),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Var),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Identifier("a"),
                        new ErrorToken("error", raw_length_of("error")),
                        new Marker(MarkerType::EQ),
                        new ErrorToken("error", raw_length_of("error")),
                        new ConstantInteger(1),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Var),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Const),
                        new ErrorToken("error", raw_length_of("error")),
                        new Identifier("a"),
                        new ErrorToken("error", raw_length_of("error")),
                        new Marker(MarkerType::EQ),
                        new ErrorToken("error", raw_length_of("error")),
                        new ConstantInteger(1),
                        new ErrorToken("error", raw_length_of("error")),
                        new Marker(MarkerType::Semicolon),
                        new ErrorToken("error", raw_length_of("error")),
                        new Identifier("a"),
                        new ErrorToken("error", raw_length_of("error")),
                        new Keyword(KeywordType::Var),
                }
        }
));