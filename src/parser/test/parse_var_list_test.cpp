
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct VarListTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(VarListTest, parse_variable_list)


INSTANTIATE_TEST_SUITE_P(Simple, VarListTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Comma),
                        new Identifier("a"),
                        new Marker(MarkerType::LBracket),
                        new Identifier("a"),
                        new Marker(MarkerType::Comma),
                        new Identifier("a"),
                        new Marker(MarkerType::LBracket),
                        new Identifier("a"),
                        new Marker(MarkerType::RBracket),
                        new Marker(MarkerType::RBracket),
                }
        }
));


struct VarListWithParenTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(VarListWithParenTest, parse_variable_list_with_paren)


INSTANTIATE_TEST_SUITE_P(Simple, VarListWithParenTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Marker(MarkerType::LParen),
                        new Identifier("a"),
                        new Marker(MarkerType::RParen),
                }
        },
        ParserTestCase{
                {
                        new Marker(MarkerType::LParen),
                        new Identifier("a"),
                        new Marker(MarkerType::Comma),
                        new Identifier("a"),
                        new Marker(MarkerType::LBracket),
                        new Identifier("a"),
                        new Marker(MarkerType::Comma),
                        new Identifier("a"),
                        new Marker(MarkerType::LBracket),
                        new Identifier("a"),
                        new Marker(MarkerType::RBracket),
                        new Marker(MarkerType::RBracket),
                        new Marker(MarkerType::RParen),
                }
        }
));


struct GuessVarListWithParenTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessVarListWithParenTest, parse_variable_list_with_paren)


INSTANTIATE_TEST_SUITE_P(Simple, GuessVarListWithParenTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RParen),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Comma),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LBracket),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Comma),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LBracket),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RBracket),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RBracket),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RParen),
                }
        }
));

