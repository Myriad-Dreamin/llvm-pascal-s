
//
// Created by kamiyoru on 2020/6/27.
//


#include "parser_test_helper.h"


struct FacTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(FacTest, parse_fac)


INSTANTIATE_TEST_SUITE_P(ConstFac, FacTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new ConstantInteger(1),
                }
        },
        ParserTestCase{
                {
                        new ConstantReal("1.", 1.),
                }
        },
        ParserTestCase{
                {
                        new ConstantBoolean(true),
                }
        },
        ParserTestCase{
                {
                        new ConstantChar('a'),
                }
        }
));

INSTANTIATE_TEST_SUITE_P(Simple, FacTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::LParen),
                        new Marker(MarkerType::RParen),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::LParen),
                        new Identifier("a"),
                        new Marker(MarkerType::RParen),
                }
        },
        ParserTestCase{
                {
                        new Marker(MarkerType::LParen),
                        new Identifier("a"),
                        new Marker(MarkerType::RParen),
                }
        }
));


INSTANTIATE_TEST_SUITE_P(Variable, FacTest, testing::Values( /* NOLINT */
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
                        new Marker(MarkerType::RBracket),
                }
        }
));


struct ExpTest : public ParserTest {
};

PARSER_SUB_BASIC_TEST_P(ExpTest, parse_exp)

INSTANTIATE_TEST_SUITE_P(Assign, ExpTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                        new Marker(MarkerType::Add),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("x"),
                        new Marker(MarkerType::Assign),
                        new Identifier("a"),
                        new Marker(MarkerType::LBracket),
                        new Identifier("i"),
                        new Marker(MarkerType::RBracket),
                }
        }
));

INSTANTIATE_TEST_SUITE_P(Arithmetic, ExpTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Add),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Mul),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::Add),
                        new Identifier("a"),
                        new Marker(MarkerType::LE),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new Identifier("a"),
                        new Marker(MarkerType::LE),
                        new Identifier("a"),
                        new Marker(MarkerType::Add),
                        new Identifier("a"),
                }
        }
));


struct GuessFacTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessFacTest, parse_fac)


INSTANTIATE_TEST_SUITE_P(ConstFac, GuessFacTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Identifier("a"),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantReal(".1", .1),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantChar('a'),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Add),
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LogicNot),
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantBoolean(true),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantBoolean(true),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RParen),
                }
        }
));


struct GuessExpressionTest : public ParserTest {
};

PARSER_GUESS_SUB_BASIC_TEST_P(GuessExpressionTest, parse_exp)


INSTANTIATE_TEST_SUITE_P(BinaryExpression, GuessExpressionTest, testing::Values( /* NOLINT */
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Add),
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Add),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Add),
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                }
        },
        ParserTestCase{
                {
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Add),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::LParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::Add),
                        new ErrorToken("a", raw_length_of("a")),
                        new ConstantInteger(1),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RParen),
                        new ErrorToken("a", raw_length_of("a")),
                        new Marker(MarkerType::RParen),
                }
        }
));


struct GuessStopExpressionTest : public ParserTest {
};

PARSER_GUESS_STOP_SUB_BASIC_TEST_P(GuessStopExpressionTest, parse_exp)


INSTANTIATE_TEST_SUITE_P(BinaryExpression, GuessStopExpressionTest, testing::Values( /* NOLINT */
        ParserTestCase{
                ParserTestCase{
                        {
                                new Identifier("a"),
                                new Marker(MarkerType::Add),
                                new Identifier("a"),
                                new Marker(MarkerType::Assign),
                        }
                },
        }
));
