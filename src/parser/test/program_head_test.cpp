
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct ProgramHeaderTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(ProgramHeaderTest, parse_program_head)


INSTANTIATE_TEST_SUITE_P(Simple, ProgramHeaderTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Program),
                        new Identifier("main"),
                }
        }
));
