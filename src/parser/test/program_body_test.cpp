
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct ProgramBodyTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(ProgramBodyTest, parse_program_body)


INSTANTIATE_TEST_SUITE_P(Simple, ProgramBodyTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Const),
                        new Identifier("a"),
                        new Marker(MarkerType::EQ),
                        new ConstantInteger(1),
                        new Marker(MarkerType::Semicolon),
                        new Keyword(KeywordType::Begin),
                        new Keyword(KeywordType::End),
                }
        }
));
