
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct VariableTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(VariableTest, parse_variable)


INSTANTIATE_TEST_SUITE_P(Simple, VariableTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::LBracket),
                        new Identifier("a"),
                        new Marker(MarkerType::RBracket),
                }
        },
        ParserTestCase{
                {
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


struct GuessVariableTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessVariableTest, parse_variable)


INSTANTIATE_TEST_SUITE_P(Simple, GuessVariableTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LBracket),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RBracket),
                }
        },
        ParserTestCase{
                {
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
                }
        }
));
