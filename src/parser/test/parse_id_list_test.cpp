
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct IdentListTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(IdentListTest, parse_id_list)


INSTANTIATE_TEST_SUITE_P(Simple, IdentListTest, testing::Values( /* NOLINT */
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
                }
        }
));


struct GuessIdentListTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessIdentListTest, parse_id_list)


INSTANTIATE_TEST_SUITE_P(Simple, GuessIdentListTest, testing::Values( /* NOLINT */
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
                        new Marker(MarkerType::Comma),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                }
        }
));
