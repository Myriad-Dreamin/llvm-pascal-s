//
// Created by kamiyoru on 2020/5/5.
//

#define VERSION "v0.1.0"

#include "compiler_option.h"
#include "compiler.h"

#include <pascal-s/parser_recursive.h>
#include <pascal-s/lexer.h>

int main(int argc, const char *argv[]) {

    CompilerOptions options(dep::_global);
    dep::parseOpts(argc, argv);
    std::istream *is = nullptr;

    if (!options.help) {
        is = &options.get_source();
    }
    FullInMemoryLexer lexer(is, &std::cerr);
    LexerProxy<FullInMemoryLexer> lexer_proxy(lexer);
    RecursiveParser<FullInMemoryLexer> parser(lexer_proxy);
    ParserProxy<RecursiveParser<FullInMemoryLexer>> parser_proxy(parser);

    Compiler<FullInMemoryLexer, RecursiveParser<FullInMemoryLexer>> compiler(
            lexer_proxy, parser_proxy,
            options, exit);

    return compiler.work(argc, argv);
}
