
#include <gtest/gtest.h>
#include <mutex>
#include <pascal-s/lexer.h>
#include <pascal-s/interface.h>
#include <pascal-s/parser.h>
#include <target/llvm.h>
#include <iostream>

const char *pascal_main_function_name = "pascal_s_main_520d4d14";

struct GenerateLLVMIRTestCase {
    const char *input;
};

class GoodGenerateLLVMIRTest : public testing::TestWithParam<GenerateLLVMIRTestCase> {

    void SetUp() override {

    };


    void TearDown() override {

    };
};

TEST_P(GoodGenerateLLVMIRTest, WillNotThrowException) /* NOLINT */
{
    auto &&param = GetParam();
    std::stringstream in(param.input);
    FullInMemoryLexer lexer(&in);

    lexer.get_all_tokens();
    lexer.reset_cursor();
    fflush(stdout);

    LexerProxy<FullInMemoryLexer> lexer_proxy(lexer);
    Parser<FullInMemoryLexer> parser(lexer_proxy);

    auto node = parser.parse();
    printAST(node);
    fflush(stdout);

    auto builder = LLVMBuilder();
    auto value = builder.code_gen(node);
    ASSERT_NE(value, nullptr);
    std::string buf;
    llvm::raw_string_ostream os(buf);

    value->print(os, true);
    std::cout << buf;
}

INSTANTIATE_TEST_SUITE_P(Simple, GoodGenerateLLVMIRTest, testing::Values( /* NOLINT */
//        GenerateLLVMIRTestCase{"program main; main := 0;"}
//        ,
//        GenerateLLVMIRTestCase{
//            "program main; begin "
//            "    main := 1 + 1; "
//            "end"},
        GenerateLLVMIRTestCase{"program main;\n"
                               "  var a: integer;\n"
                               "  begin\n"
                               "    read_int64(a);\n"
                               "    write_int64(a);\n"
                               "    main := 0;\n"
                               "  end"}
//                                   ,
//        GenerateLLVMIRTestCase{"program main;\n"
//                               " begin\n"
//                               "   write_char('h');\n"
//                               //               "   write_char('e');\n"
//                               //               "   write_char('l');\n"
//                               //               "   write_char('l');\n"
//                               //               "   write_char('o');\n"
//                               //               "   write_char(' ');\n"
//                               //               "   write_char('w');\n"
//                               //               "   write_char('o');\n"
//                               //               "   write_char('r');\n"
//                               //               "   write_char('l');\n"
//                               //               "   write_char('d');\n"
//                               "   main := 0;\n"
//                               " end"}
));


