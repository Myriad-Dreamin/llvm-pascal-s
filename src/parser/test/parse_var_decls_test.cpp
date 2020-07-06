
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct VarDeclsTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(VarDeclsTest, parse_var_decls)


INSTANTIATE_TEST_SUITE_P(Simple, VarDeclsTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Var),
                        new Identifier("a"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Var),
                        new Identifier("a"),
                        new Marker(MarkerType::Comma),
                        new Identifier("b"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("c"),
                        new Marker(MarkerType::Colon),
                        new Keyword(KeywordType::Integer),
                        new Marker(MarkerType::Semicolon),
                }
        }
));


struct GuessVarDeclsTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessVarDeclsTest, parse_var_decls)


INSTANTIATE_TEST_SUITE_P(Simple, GuessVarDeclsTest, testing::Values( /* NOLINT */
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
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Semicolon),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Var),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")), // 5
                        new Marker(MarkerType::Comma),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("b"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Colon),
                        new ErrorToken("a", raw_length_of("a")), // 11
                        new Keyword(KeywordType::Integer),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Semicolon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("c"),
                        new ErrorToken("a", raw_length_of("a")), // 17
                        new Marker(MarkerType::Colon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Integer),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Semicolon),
                }
        }
));
