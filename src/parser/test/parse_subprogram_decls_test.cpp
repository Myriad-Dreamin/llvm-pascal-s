
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct SubprogramDelcsTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(SubprogramDelcsTest, parse_subprogram_decls)


INSTANTIATE_TEST_SUITE_P(Simple, SubprogramDelcsTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
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
                }
        }
));
