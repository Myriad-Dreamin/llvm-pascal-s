
#include <gtest/gtest.h>
#include <pascal-s/lexer.h>
#include <pascal-s/features.h>
#include <pascal-s/lib/stream_file.h>

struct ShowErrorTestCase {
    const char *input;
    const char *expected;
};

class GoodShowErrorTest : public testing::TestWithParam<ShowErrorTestCase> {
};

class CFile {
    FILE *f;

public:
    explicit CFile(FILE *f) : f(f) {}
};

TEST_P(GoodShowErrorTest, WillNotThrowException) /* NOLINT */
{
    auto &param = GetParam();
    std::stringstream in(param.input);
    FullInMemoryLexer lexer(&in);

    lexer.get_all_tokens();
    ASSERT_TRUE(lexer.has_error());

    pascal_s::CPPStreamFile fin(in);
    FileProxy<pascal_s::CPPStreamFile> fp(fin);
    WriterProxy<std::ostream> os(std::cout);

    for (auto e : lexer.get_all_errors()) {
        feature::format_line_column_error(fp, ErrorProxy<ErrorToken>(*e), os);
    }
}

INSTANTIATE_TEST_SUITE_P(TestIdentifiers, GoodShowErrorTest, testing::Values( /* NOLINT */
        ShowErrorTestCase{"1a"},
        ShowErrorTestCase{"     1a"},
        ShowErrorTestCase{"     1a      "},
        ShowErrorTestCase{"     'xx'      "}
));
