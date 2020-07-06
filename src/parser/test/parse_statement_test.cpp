
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct StatementTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(StatementTest, parse_statement)


INSTANTIATE_TEST_SUITE_P(Expression, StatementTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                }
        }
));


INSTANTIATE_TEST_SUITE_P(CompoundStatement, StatementTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::Begin),
                        new Identifier("a"),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                        new Marker(MarkerType::LParen),
                        new Marker(MarkerType::RParen),
                        new Marker(MarkerType::Semicolon),
                        new Identifier("a"),
                        new Keyword(KeywordType::End),
                }
        }
));


INSTANTIATE_TEST_SUITE_P(ForStatement, StatementTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::For),
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                        new Keyword(KeywordType::To),
                        new Identifier("a"),
                        new Keyword(KeywordType::Do),
                        new Identifier("a"),
                }
        }
));


INSTANTIATE_TEST_SUITE_P(IfElseStatement, StatementTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Keyword(KeywordType::If),
                        new Identifier("a"),
                        new Keyword(KeywordType::Then),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Keyword(KeywordType::If),
                        new Identifier("a"),
                        new Keyword(KeywordType::Then),
                        new Keyword(KeywordType::For),
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                        new Keyword(KeywordType::To),
                        new Identifier("a"),
                        new Keyword(KeywordType::Do),
                        new Identifier("a"),
                        new Keyword(KeywordType::Else),
                        new Identifier("a"),
                }
        }
));


struct GuessStatementTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessStatementTest, parse_statement)


INSTANTIATE_TEST_SUITE_P(CompoundStatement, GuessStatementTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Begin),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::End),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Begin),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Semicolon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Assign),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Semicolon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Assign),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Semicolon),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::End),
                }
        }
));


INSTANTIATE_TEST_SUITE_P(ForStatement, GuessStatementTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::For),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Assign),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::To),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Do),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                }
        }
));


INSTANTIATE_TEST_SUITE_P(IfElseStatement, GuessStatementTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::If),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Then),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::If),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Then),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::For),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Assign),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::To),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Do),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                        new ErrorToken("a", raw_length_of("a")),
                        new Keyword(KeywordType::Else),
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                }
        }
));



