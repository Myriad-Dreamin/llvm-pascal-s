
#include <gtest/gtest.h>
#include <pascal-s/token.h>
#include <pascal-s/lexer.h>
#include "token_helper.h"

// LexerGetNextToken Test
// 测试 next_token


struct LexerGetNextTokenTestCase {

    const char *input = nullptr;
    Token *expected = nullptr;
};

template<typename TestingLexer>
struct GoodLexerGetNextTokenTest : public testing::TestWithParam<LexerGetNextTokenTestCase> {
    using Lexer = TestingLexer;

    void TearDown() override {
        deleteToken(GetParam().expected);
    };

    static auto TestMain() {
        auto &&param = GetParam();
        std::stringstream in(param.input);
        Lexer lexer(&in);
        auto tok = lexer.next_token();
        ASSERT_NE(tok, nullptr);
        ASSERT_TOKEN_EQUAL(tok, param.expected);
    }

    [[maybe_unused]] static auto TestValues() noexcept {
        return testing::Values(
                LexerGetNextTokenTestCase{"a", new Identifier("a")},
                LexerGetNextTokenTestCase{"a ", new Identifier("a")}
        );
    }
};

#define GetNextTokenTest(LexerName) using Good##LexerName##GetNextTokenTest = GoodLexerGetNextTokenTest<LexerName>;\
    TEST_P(Good##LexerName##GetNextTokenTest, WillNotThrowException) /* NOLINT */\
        {Good##LexerName##GetNextTokenTest::TestMain();}\
    INSTANTIATE_TEST_SUITE_P(TestIdentifiers, Good##LexerName##GetNextTokenTest,\
        Good##LexerName##GetNextTokenTest::TestValues()) /* NOLINT */

GetNextTokenTest(FullInMemoryLexer);


// LexerGetNextToken Test
// 测试 next_token

struct LexerGetAllTokensTestCase {
    const char *input;
    std::vector<Token *> *expected;
    uint16_t lo;
};

class LexerGetAllTokensTest : public testing::TestWithParam<LexerGetAllTokensTestCase> {

    void TearDown() override {
        auto &tokens = GetParam().expected;
        for (auto tok: *tokens) {
            deleteToken(tok);
        }
        tokens->clear();
        delete tokens;
    };
};

class GoodLexerGetAllTokensTest : public LexerGetAllTokensTest {
};


void out_tokens(Lexer::token_container &vec) {
    for (auto i : vec) {
        std::cout << convertToString(i) << " ";
    }
}


TEST_P(GoodLexerGetAllTokensTest, WillNotThrowException) /* NOLINT */ {
    auto &&param = GetParam();
    std::stringstream in(param.input);
    FullInMemoryLexer lexer(&in);
    lexer.setOption(param.lo);
    auto tokens = lexer.get_all_tokens();
    if (tokens.size() - param.expected->size()) out_tokens(tokens);
    ASSERT_EQ(tokens.size(), param.expected->size());
    for (size_t i = 0; i < tokens.size(); i++) {
        ASSERT_TOKEN_EQUAL(tokens[i], (*param.expected)[i]);
    }
}

#define CaseExpectIdentifier(source, matched) LexerGetAllTokensTestCase{source, new std::vector<Token *>({new Identifier(matched)})}

INSTANTIATE_TEST_SUITE_P(IdentifierRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectIdentifier("a ", "a"),
        CaseExpectIdentifier("A ", "a"),
        CaseExpectIdentifier("A1 ", "a1"),
        CaseExpectIdentifier("_a ", "_a"),
        CaseExpectIdentifier("_1 ", "_1")
));

#undef CaseExpectIdentifier

#define CaseExpectKeyword(source, matched) LexerGetAllTokensTestCase{source, new std::vector<Token *>({new Keyword(matched)})}

INSTANTIATE_TEST_SUITE_P(KeywordRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectKeyword("function ", KeywordType::Function),
        CaseExpectKeyword("Function ", KeywordType::Function),
        CaseExpectKeyword("FUNCTION ", KeywordType::Function),
        CaseExpectKeyword("Program ", KeywordType::Program),
        CaseExpectKeyword("Var ", KeywordType::Var),
        CaseExpectKeyword("Procedure ", KeywordType::Procedure),
        CaseExpectKeyword("Begin ", KeywordType::Begin),
        CaseExpectKeyword("End ", KeywordType::End),
        CaseExpectKeyword("Array ", KeywordType::Array),
        CaseExpectKeyword("Integer ", KeywordType::Integer),
        CaseExpectKeyword("Real ", KeywordType::Real),
        CaseExpectKeyword("Boolean ", KeywordType::Boolean),
        CaseExpectKeyword("Char ", KeywordType::Char),
        CaseExpectKeyword("If ", KeywordType::If),
        CaseExpectKeyword("Then ", KeywordType::Then),
        CaseExpectKeyword("Else ", KeywordType::Else),
        CaseExpectKeyword("For ", KeywordType::For),
        CaseExpectKeyword("To ", KeywordType::To),
        CaseExpectKeyword("Do ", KeywordType::Do),
        CaseExpectKeyword("Of ", KeywordType::Of),
        CaseExpectKeyword("Read ", KeywordType::Read),
        CaseExpectKeyword("Write ", KeywordType::Write),
        CaseExpectKeyword("Const ", KeywordType::Const)
));

#undef CaseExpectKeyword

#define CaseExpectMarker(source, matched) LexerGetAllTokensTestCase{source, new std::vector<Token *>({new Marker(matched)})}

INSTANTIATE_TEST_SUITE_P(MarkerRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectMarker(".. ", MarkerType::Range),
        CaseExpectMarker("<> ", MarkerType::NEQ),
        CaseExpectMarker("<= ", MarkerType::LE),
        CaseExpectMarker(">= ", MarkerType::GE),
        CaseExpectMarker("< ", MarkerType::LT),
        CaseExpectMarker("= ", MarkerType::EQ),
        CaseExpectMarker("> ", MarkerType::GT),
        CaseExpectMarker("+ ", MarkerType::Add),
        CaseExpectMarker("- ", MarkerType::Sub),
        CaseExpectMarker("* ", MarkerType::Mul),
        CaseExpectMarker("/ ", MarkerType::Div),

        CaseExpectMarker("( ", MarkerType::LParen),
        CaseExpectMarker(") ", MarkerType::RParen),
        CaseExpectMarker("[ ", MarkerType::LBracket),
        CaseExpectMarker("] ", MarkerType::RBracket),

        CaseExpectMarker(":= ", MarkerType::Assign),
        CaseExpectMarker(", ", MarkerType::Comma),
        CaseExpectMarker(". ", MarkerType::Dot),
        CaseExpectMarker("; ", MarkerType::Semicolon),
        CaseExpectMarker(": ", MarkerType::Colon),

        CaseExpectMarker("div ", MarkerType::Div),
        CaseExpectMarker("mod ", MarkerType::Mod),
        CaseExpectMarker("and ", MarkerType::LogicAnd),
        CaseExpectMarker("or ", MarkerType::LogicOr),
        CaseExpectMarker("not ", MarkerType::LogicNot)
));

#undef CaseExpectMarker

//#define CaseExpectConstantString(source, matched) LexerGetAllTokensTestCase{source, new std::vector<Token *>({new ConstantString()})}
//
//INSTANTIATE_TEST_SUITE_P(ConstantStringRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
//));
//
//#undef CaseExpectConstantString

#define CaseExpectConstantChar(source, parsed) \
    LexerGetAllTokensTestCase{source, new std::vector<Token *>({new ConstantChar(parsed)})}

INSTANTIATE_TEST_SUITE_P(ConstantCharRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectConstantChar("'a' ", 'a'),
        CaseExpectConstantChar("'A' ", 'A'),
        CaseExpectConstantChar("'0' ", '0'),
        CaseExpectConstantChar("'x' ", 'x'),
        CaseExpectConstantChar("'\\0' ", '\x00'),
        CaseExpectConstantChar("'\\9' ", '\x09'),
        CaseExpectConstantChar("'\\00' ", '\x00'),
        CaseExpectConstantChar("'\\11' ", '\x11'),
        CaseExpectConstantChar("'\\99' ", '\x99'),
        CaseExpectConstantChar("'\\xff' ", '\xff'),
        CaseExpectConstantChar("'\\cf' ", '\xcf'),
        CaseExpectConstantChar("'\\a' ", '\a'),
        CaseExpectConstantChar("'\\b' ", '\b'),
        CaseExpectConstantChar("'\\f' ", '\f'),
        CaseExpectConstantChar("'\\n' ", '\n'),
        CaseExpectConstantChar("'\\r' ", '\r'),
        CaseExpectConstantChar("'\\t' ", '\t'),
        CaseExpectConstantChar("'\\v' ", '\v'),
        CaseExpectConstantChar("'\\'' ", '\''),
        CaseExpectConstantChar("'\\\\' ", '\\'),
        CaseExpectConstantChar("' ' ", ' ')
));

#undef CaseExpectConstantChar

#define CaseExpectConstantReal(source, matched, parsed) LexerGetAllTokensTestCase{source, new std::vector<Token *>({new ConstantReal(matched, parsed)})}

INSTANTIATE_TEST_SUITE_P(ConstantRealRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectConstantReal(".1 ", ".1", .1),
        CaseExpectConstantReal(".11 ", ".11", .11),
        CaseExpectConstantReal("1.1 ", "1.1", 1.1),
        CaseExpectConstantReal("1.11 ", "1.11", 1.11),
        CaseExpectConstantReal("11.1 ", "11.1", 11.1),
        CaseExpectConstantReal("11.11 ", "11.11", 11.11),
        CaseExpectConstantReal(".0 ", ".0", .0),
        CaseExpectConstantReal(".00 ", ".00", .00),
        CaseExpectConstantReal("0.0 ", "0.0", 0.0),
        CaseExpectConstantReal("0.00 ", "0.00", 0.00),
        CaseExpectConstantReal("00.0 ", "00.0", 00.0),
        CaseExpectConstantReal("00.00 ", "00.00", 00.00)
));

#undef CaseExpectConstantReal

#define CaseExpectConstantInteger(source, matched) LexerGetAllTokensTestCase{source, new std::vector<Token *>({new ConstantInteger(matched)})}

INSTANTIATE_TEST_SUITE_P(ConstantIntegerRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectConstantInteger("1 ", 1),
        CaseExpectConstantInteger("01 ", 1),
        CaseExpectConstantInteger("11 ", 11)
));

#undef CaseExpectConstantInteger

#define CaseExpectConstantBoolean(source, matched) LexerGetAllTokensTestCase{source, new std::vector<Token *>({new ConstantBoolean(matched)})}

INSTANTIATE_TEST_SUITE_P(ConstantBooleanRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectConstantBoolean("false ", false),
        CaseExpectConstantBoolean("true ", true)
));

#undef CaseExpectConstantBoolean

#define CaseExpectComment(source) LexerGetAllTokensTestCase{source, new std::vector<Token *>()}
#define CaseExpectNoSkipComment(source, matched) LexerGetAllTokensTestCase{\
    source, new std::vector<Token *>({new Comment(matched)}), Lexer::LexerOptionLexComment}

INSTANTIATE_TEST_SUITE_P(ConstantCommentRegex, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        CaseExpectComment("{{ \\} \\} \\\\\\}  }} "),
        CaseExpectComment("{} "),
//        CaseExpectComment("{\x00} "),
        CaseExpectComment("{{}} "),
        CaseExpectComment("{{\\}}} "),
        CaseExpectComment("{ efg { abc \a }} "),
        CaseExpectNoSkipComment("{{ \\} \\} \\\\\\}  }} ", "{{ } } \\}  }}"),
        CaseExpectNoSkipComment("{} ", "{}"),
//        CaseExpectNoSkipComment("{\x00} ", "{\x00}"),
        CaseExpectNoSkipComment("{{}} ", "{{}}"),
        CaseExpectNoSkipComment("{{\\}}}", "{{}}}"),
        CaseExpectNoSkipComment("{ efg { abc \a }} ", "{ efg { abc \a }}")
));

#undef CaseExpectComment

//enum class TokenType {
//    ConstantString = 2,
//    Nullptr = 9,
//};

//enum class KeywordType {

//    Div,
//    Mod,
//    And,
//    Or,
//    Not,
//    Length
//};

INSTANTIATE_TEST_SUITE_P(Stream, GoodLexerGetAllTokensTest, testing::Values( /* NOLINT */
        LexerGetAllTokensTestCase{"a b",
                                  new std::vector<Token *>({new Identifier("a"), new Identifier("b")})},
        LexerGetAllTokensTestCase{"a //b\n a",
                                  new std::vector<Token *>({
                                                                   new Identifier("a"),
                                                                   new Identifier("a")})},
        LexerGetAllTokensTestCase{"To", new std::vector<Token *>({new Keyword(get_keyword_type("to"))})},
        LexerGetAllTokensTestCase{
                "div mod And Or Not read write",
                new std::vector<Token *>(
                        {
                                new Marker(get_marker_type("div")),
                                new Marker(get_marker_type("mod")),
                                new Marker(get_marker_type("and")),
                                new Marker(get_marker_type("or")),
                                new Marker(get_marker_type("not")),
                                new Keyword(get_keyword_type("write")),
                                new Keyword(get_keyword_type("read"))})},
        LexerGetAllTokensTestCase{"'a''a'", new std::vector<Token *>(
                {
                        new ConstantChar('a'),
                        new ConstantChar('a'),
                })},
        LexerGetAllTokensTestCase{"{}}", new std::vector<Token *>(
                {
                        new ErrorToken("}"),
                })},
        LexerGetAllTokensTestCase{"{\\\\}}", new std::vector<Token *>(
                {
                        new ErrorToken("}"),
                })},
        LexerGetAllTokensTestCase{"{\\}}", new std::vector<Token *>(
                {
                })},
        LexerGetAllTokensTestCase{"read('h', 'e', 'l', 'l', 'o');", new std::vector<Token *>(
                {
                        new Keyword(KeywordType::Read),
                        new Marker(MarkerType::LBracket),
                        new ConstantChar('h'),
                        new Marker(MarkerType::Comma),
                        new ConstantChar('e'),
                        new Marker(MarkerType::Comma),
                        new ConstantChar('l'),
                        new Marker(MarkerType::Comma),
                        new ConstantChar('l'),
                        new Marker(MarkerType::Comma),
                        new ConstantChar('o'),
                        new Marker(MarkerType::RBracket),
                        new Marker(MarkerType::Semicolon),
                })}
));

class BadLexerGetAllTokensTest : public LexerGetAllTokensTest {
};


TEST_P(BadLexerGetAllTokensTest, WillNotThrowException) /* NOLINT */
{
    auto &&param = GetParam();
    std::stringstream in(param.input);
    FullInMemoryLexer lexer(&in);
    auto tokens = lexer.get_all_tokens();
    ASSERT_EQ(tokens.size(), param.expected->size());
    for (size_t i = 0; i < tokens.size(); i++) {
        ASSERT_TOKEN_EQUAL(tokens[i], (*param.expected)[i]);
    }
    ASSERT_TRUE(lexer.has_error());
}


INSTANTIATE_TEST_SUITE_P(Simple, BadLexerGetAllTokensTest, testing::Values( /* NOLINT */
        LexerGetAllTokensTestCase{"1a",
                                  new std::vector<Token *>(
                                          {new ErrorToken("1a")})}
));
