
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct IdentListTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(IdentListTest, parse_param_list)


INSTANTIATE_TEST_SUITE_P(Simple, IdentListTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Var),
                        new Identifier("a"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Var),
                        new Identifier("a"),
                        new Marker(MarkerType::Comma),
                        new Identifier("a"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Real),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("a"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Boolean),
                }
        }
));


struct GuessIdentListTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessIdentListTest, parse_param_list)


INSTANTIATE_TEST_SUITE_P(Simple, GuessIdentListTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Var),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Colon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Integer),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Colon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Integer),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Var),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Comma),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Colon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Real),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Semicolon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Colon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Boolean),
                }
        }
));
