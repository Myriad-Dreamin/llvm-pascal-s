
//
// Created by kamiyoru on 2020/6/27.
//

#ifndef PASCAL_S_PARSER_TEST_HELPER_H
#define PASCAL_S_PARSER_TEST_HELPER_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <pascal-s/token.h>
#include <pascal-s/lexer_mock.h>
#include <pascal-s/parser_recursive.h>

struct ParserTestCase {

    std::vector<Token *> token_stream;
//    ASTNode *expected = nullptr;
};

struct ParserTest : public testing::TestWithParam<ParserTestCase> {

    void TearDown() override {
        for (auto tok : GetParam().token_stream) deleteToken(tok);
    };
};


#define PARSER_BASIC_TEST_P(ParserTestName, ParseRespFunc) TEST_P(ParserTestName, BasicWillNotThrowException) /* NOLINT */ \
{ \
    auto &&param = GetParam(); \
    MockLexer lexer(param.token_stream); \
    LexerProxy <MockLexer> lexer_proxy(lexer); \
    RecursiveParser parser(lexer_proxy); \
    auto ast = parser.ParseRespFunc(); \
    ASSERT_EQ(lexer.current_token_cursor, lexer.token_stream.size()) <<  "not consumed " \
        << lexer.current_token_cursor << " " << lexer.token_stream.size(); \
    if (!parser.errors.empty()) { \
        std::cout << parser.errors[0]->what() << std::endl; \
        ASSERT_TRUE(false) << "abort";\
        \
    } \
    ASSERT_NE(ast, nullptr); \
 \
    deleteAST(ast); \
}

#define PARSER_SUB_BASIC_TEST_P(ParserTestName, ParseRespFunc) TEST_P(ParserTestName, BasicWillNotThrowException) /* NOLINT */ \
{ \
    auto &&param = GetParam(); \
    MockLexer lexer(param.token_stream); \
    LexerProxy <MockLexer> lexer_proxy(lexer); \
    RecursiveParser parser(lexer_proxy); \
    parser.next_token();\
    auto ast = parser.ParseRespFunc(); \
    ASSERT_EQ(lexer.current_token_cursor, lexer.token_stream.size()) <<  "not consumed " \
        << lexer.current_token_cursor << " " << lexer.token_stream.size(); \
    if (!parser.errors.empty()) { \
        std::cout << parser.errors[0]->what() << std::endl; \
        ASSERT_TRUE(false) << "abort";\
    } \
    ASSERT_NE(ast, nullptr); \
 \
    deleteAST(ast); \
}

#define PARSER_GUESS_P_SUB_BASIC_TEST_P(ParserTestName, ParseRespFunc, ext) TEST_P(ParserTestName, BasicWillNotThrowException) /* NOLINT */ \
{ \
    auto &&param = GetParam(); \
    MockLexer lexer(param.token_stream); \
    LexerProxy <MockLexer> lexer_proxy(lexer); \
    RecursiveParser parser(lexer_proxy); \
    parser.next_token();\
    auto ast = parser.ParseRespFunc(); \
    ASSERT_EQ(lexer.current_token_cursor, lexer.token_stream.size()) <<  "not consumed " \
        << lexer.current_token_cursor << " " << lexer.token_stream.size(); \
    if (parser.errors.empty()) { \
        ASSERT_TRUE(false) << "abort";\
    } \
    ASSERT_##ext(parser.current_token, nullptr); \
    ASSERT_NE(ast, nullptr); \
 \
    deleteAST(ast); \
}

#define PARSER_GUESS_SUB_BASIC_TEST_P(ParserTestName, ParseRespFunc) PARSER_GUESS_P_SUB_BASIC_TEST_P(ParserTestName, ParseRespFunc, EQ)
#define PARSER_GUESS_STOP_SUB_BASIC_TEST_P(ParserTestName, ParseRespFunc) \
    PARSER_GUESS_P_SUB_BASIC_TEST_P(ParserTestName, ParseRespFunc, NE)


#define raw_length_of(rs) (sizeof(rs) - 1)

#endif