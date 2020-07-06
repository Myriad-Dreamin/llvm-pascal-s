//
// Created by kamiyoru on 2020/7/2.
//


#include <gtest/gtest.h>
#include <pascal-s/llvm-ast.h>

struct AstPosTestCase {
};

class GoodAstPosTest : public testing::TestWithParam<AstPosTestCase> {

    void TearDown() override {
    };
};

#define ASSERT_NODE_WITH_POS(node) do {\
    auto node_assert_pos = node.visit_pos();\
    ASSERT_EQ(node_assert_pos->line, node.line);\
    ASSERT_EQ(node_assert_pos->offset, node.offset);\
    ASSERT_EQ(node_assert_pos->column, node.column);\
    ASSERT_EQ(node_assert_pos->length, node.length);} while(0)


TEST_P(GoodAstPosTest, WillNotThrowException) /* NOLINT */
{

    auto node = ast::Node(ast::Type::Unknown);
    node.offset = 1;
    node.length = 2;
    node.line = 3;
    node.column = 4;

    ASSERT_NODE_WITH_POS(node);
}


INSTANTIATE_TEST_SUITE_P(Basic, GoodAstPosTest, testing::Values( /* NOLINT */
        AstPosTestCase{}
));

class GoodAstPosCopyTest : public testing::TestWithParam<AstPosTestCase> {

    void TearDown() override {
    };
};


TEST_P(GoodAstPosCopyTest, WillNotThrowException) /* NOLINT */
{
    auto raw_node = ast::Node(ast::Type::Unknown);
    raw_node.offset = 1;
    raw_node.length = 2;
    raw_node.line = 3;
    raw_node.column = 4;

    auto raw_tok = Token();
    raw_tok.offset = 1;
    raw_tok.length = 2;
    raw_tok.line = 3;
    raw_tok.column = 4;

    auto node = ast::Node(ast::Type::Unknown);
    copy_pos_any(node, raw_tok);
    ASSERT_NODE_WITH_POS(node);

    node = ast::Node(ast::Type::Unknown);
    copy_pos_any(node, &raw_tok);
    ASSERT_NODE_WITH_POS(node);

    node = ast::Node(ast::Type::Unknown);
    copy_pos_any(node, raw_node);
    ASSERT_NODE_WITH_POS(node);

    node = ast::Node(ast::Type::Unknown);
    copy_pos_any(node, &raw_node);
    ASSERT_NODE_WITH_POS(node);

    node = ast::Node(ast::Type::Unknown);
    copy_pos_any(&node, raw_node);
    ASSERT_NODE_WITH_POS(node);

    node = ast::Node(ast::Type::Unknown);
    copy_pos_any(&node, &raw_node);
    ASSERT_NODE_WITH_POS(node);
}


INSTANTIATE_TEST_SUITE_P(Basic, GoodAstPosCopyTest, testing::Values( /* NOLINT */
        AstPosTestCase{}
));
