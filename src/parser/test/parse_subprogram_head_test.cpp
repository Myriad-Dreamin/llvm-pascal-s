
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct SubprogramHeaderTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(SubprogramHeaderTest, parse_subprogram_head)


INSTANTIATE_TEST_SUITE_P(Simple, SubprogramHeaderTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Procedure),
                        new Identifier("main"),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Procedure),
                        new Identifier("main"),
                        new Marker(MarkerType::LParen),
                        new Marker(MarkerType::RParen),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Function),
                        new Identifier("main"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Function),
                        new Identifier("main"),
                        new Marker(MarkerType::LParen),
                        new Marker(MarkerType::RParen),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                }
        }
));
