
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct TypeTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(TypeTest, parse_type)


INSTANTIATE_TEST_SUITE_P(Simple, TypeTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Integer),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Real),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Char),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Boolean),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Array),
                        new Marker(MarkerType::LBracket),
                        new ConstantInteger(0),
                        new Marker(MarkerType::Range),
                        new ConstantInteger(1),
                        new Marker(MarkerType::RBracket),
                        new Keyword(KeywordType::Of),
                        new Keyword(KeywordType::Integer),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Array),
                        new Marker(MarkerType::LBracket),
                        new ConstantInteger(0),
                        new Marker(MarkerType::Range),
                        new ConstantInteger(1),
                        new Marker(MarkerType::Comma),
                        new ConstantInteger(0),
                        new Marker(MarkerType::Range),
                        new ConstantInteger(1),
                        new Marker(MarkerType::RBracket),
                        new Keyword(KeywordType::Of),
                        new Keyword(KeywordType::Integer),
                }
        }
));

struct GuessTypeTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessTypeTest, parse_type)

INSTANTIATE_TEST_SUITE_P(Guessing, GuessTypeTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("integers", raw_length_of("integers")),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("booleans", raw_length_of("booleans")),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("cha", raw_length_of("cha")),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("rea", raw_length_of("rea")),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("arr", raw_length_of("arr")),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Marker(MarkerType::LBracket),
                        new ConstantInteger(0),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Marker(MarkerType::Range),
                        new ConstantInteger(1),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Marker(MarkerType::RBracket),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Keyword(KeywordType::Of),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new ErrorToken("integers", raw_length_of("integers")),
                }
        },
        ParserTestCase{
                // arr wrong_token [ 0.1 wrong_token .. 1.1 wrong_token ] wrong_token of wrong_token integers
                // ->
                // array           [ 0               .. 1               ]             of             integer
                {
                        new ErrorToken("arr", raw_length_of("arr")),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Marker(MarkerType::LBracket),
                        new ConstantReal("0.1", 0.1),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Marker(MarkerType::Range),
                        new ConstantReal("1.1", 1.1),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Marker(MarkerType::RBracket),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new Keyword(KeywordType::Of),
                        new ErrorToken("wrong_token", raw_length_of("wrong_token")),
                        new ErrorToken("integers", raw_length_of("integers")),
                }
        }
));
