
#include <dep/opt.h>
#include <gtest/gtest.h>
#include <fmt/core.h>

#include <utility>

using applyF = std::function<void(dep::OptParser &)>;

template<typename T>
auto applyOpt(T &&opt) {
    return [&](dep::OptParser &parser) {
        parser.addOpts(std::forward<T>(opt));
    };
}

struct OptParseTestCase {
    int argc;
    const char **argv;

    std::vector<applyF> opts;

    std::vector<std::tuple<dep::StringOpt *, std::string *, const char *>> str_cases;

    [[maybe_unused]] OptParseTestCase &str_case(const char *k, const char *v, const char *default_v = "") {
        auto storage = new std::string;
        auto opt = new dep::StringOpt(k, storage, "", "", default_v);
        opts.emplace_back(applyOpt(*opt));
        str_cases.emplace_back(opt, storage, v);
        return *this;
    }

    std::vector<std::tuple<dep::Int64Opt *, int64_t *, int64_t>> int64_cases;

    [[maybe_unused]] OptParseTestCase &int64_case(const char *k, int64_t v, int64_t default_v = 0) {
        auto storage = new int64_t;
        auto opt = new dep::Int64Opt(k, storage, "", "", default_v);
        opts.emplace_back(applyOpt(*opt));
        int64_cases.emplace_back(opt, storage, v);
        return *this;
    }
};

class OptParseTest : public testing::TestWithParam<OptParseTestCase> {
};

[[maybe_unused]] const char *case1[] = {"--a=b"};
[[maybe_unused]] const char *case2[] = {"--a="};
[[maybe_unused]] const char *case3[] = {"--b=1"};

#define genCaseOf(i) OptParseTestCase{sizeof(case##i)/sizeof(case##i[0]), case##i}

TEST_P(OptParseTest, WillNotThrowException) /* NOLINT */
{
    auto &param = GetParam();
    dep::OptParser parser;

    for (auto &f: param.opts) {
        f(parser);
    }

    int argc = GetParam().argc;
    const char **argv = GetParam().argv;
    parser.parseOpts(argc, argv);

    for (auto &str_case : param.str_cases) {
        const auto &opt = std::get<0>(str_case);
        const auto &storage = std::get<1>(str_case);
        const auto &expected = std::get<2>(str_case);
        ASSERT_STREQ(storage->c_str(), expected);
        delete opt;
        delete storage;
    }

    for (auto &int64_case : param.int64_cases) {
        const auto &opt = std::get<0>(int64_case);
        const auto &storage = std::get<1>(int64_case);
        const auto &expected = std::get<2>(int64_case);
        ASSERT_EQ(*storage, expected);
        delete opt;
        delete storage;
    }
}

INSTANTIATE_TEST_SUITE_P(WillNotThrowException, OptParseTest, testing::Values( /* NOLINT */
        genCaseOf(1)
                .str_case("a", "b", "_a_default_value"),
        genCaseOf(2)
                .str_case("a", "", "_a_default_value"),
        genCaseOf(3)
                .str_case("a", "_a_default_value", "_a_default_value"),
        genCaseOf(3)
                .str_case("b", "1", "1"),
        genCaseOf(3)
                .int64_case("b", 1, -1)
));

class OptParseTestWillThrowException : public testing::TestWithParam<OptParseTestCase> {
};

TEST_P(OptParseTestWillThrowException, WillThrowException) /* NOLINT */
{
    auto &param = GetParam();
    dep::OptParser parser;

    for (auto &f: param.opts) {
        f(parser);
    }

    int argc = GetParam().argc;
    const char **argv = GetParam().argv;

    ASSERT_THROW(parser.parseOpts(argc, argv), dep::InvalidConversion);
}

INSTANTIATE_TEST_SUITE_P(WillThrowException, OptParseTestWillThrowException, testing::Values( /* NOLINT */
        genCaseOf(2)
                .int64_case("a", 0, -1)
));

