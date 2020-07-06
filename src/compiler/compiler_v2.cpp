//
// Created by kamiyoru on 2020/5/5.
//

#define VERSION "v0.2.0"

#include "compiler_option.h"
#include "compiler.h"

#include <pascal-s/parser.h>
#include <pascal-s/lexer.h>

template<typename Lexer>
extern Parser *parser_factory(Lexer &);
//#include <pascal-s/parser.h>
//#include <target/llvm.h>

int main(int argc, const char *argv[]) {

    CompilerOptions options(dep::_global);
    dep::parseOpts(argc, argv);
    std::istream *is = nullptr;

    if (!options.help) {
        is = &options.get_source();
    }
    FullInMemoryLexer lexer(is, &std::cerr);
    Parser *parser = parser_factory(lexer);

    Compiler<FullInMemoryLexer, Parser> compiler(
            LexerProxy<FullInMemoryLexer>(lexer), ParserProxy<Parser>(*parser),
            options,[&](int code) {
        delete parser;
        exit(code);
    });

    int code = compiler.work(argc, argv);
    delete parser;
    return code;
}
