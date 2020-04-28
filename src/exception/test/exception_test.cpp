
#include <gtest/gtest.h>
#include <pascal-s/token.h>
#include "fmt/core.h"

struct ExceptionTestCase {
    const Token &token;
};

class ExceptionTest : public testing::TestWithParam<ExceptionTestCase> {
};

TEST_P(ExceptionTest, WillNotThrowException) /* NOLINT */
{
    auto &token = GetParam().token;
    ASSERT_EQ(
            fmt::format("runtime reinterpret token error: unknown type: {}", token.type),
            RuntimeReinterpretTokenException(&token).what());
}

INSTANTIATE_TEST_SUITE_P(TestIdentifiers, ExceptionTest, testing::Values( /* NOLINT */
        ExceptionTestCase{Keyword(KeywordType::If)},
        ExceptionTestCase{Token()}
));