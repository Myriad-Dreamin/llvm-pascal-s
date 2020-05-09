//
// Created by kamiyoru on 2020/5/5.
//

#include <dep/opt.h>
#include <iostream>
#include <fstream>
#include <fmt/core.h>
#include <pascal-s/features.h>
#include <pascal-s/interface.h>
#include <pascal-s/lexer.h>
#include <pascal-s/parser.h>
#include <target/llvm.h>
#include <target/task.h>
#include <functional>
#include <utility>


#define VERSION "v0.1.0"

string_view_return basename(string_view s) {
    for (size_t i = s.length() - 1; i >= 0; i--) {
        if (s[i] == '\\' || s[i] == '/') {
            return s.substr(i + 1);
        }
    }
    return s;
}

static const char *cat_default = "";
static const char *cat_compile_options = "compile options";

extern int target_compile(int argc, const char **pString, CompilerTargetTask *task);

struct CompilerOptions {
    bool help = false;
    bool version = false;
    bool env = false;
    std::string wk_dir;
    std::string source_path;
    std::string out_path;
    std::string out_with_ir;
    std::string out_with_token;

    explicit CompilerOptions(dep::OptParser &parser) {
        parser.addOpts("help", &help, "Print help message", cat_default, false, "h");
        parser.addOpts("version", &version, "Print Version", cat_default, false, "");
        parser.addOpts("env", &env, "Print the environment of pascal", cat_default, false, "");
        parser.addOpts("work-dir", &wk_dir, "Working directory",
                       cat_compile_options, ".", "wd");
        parser.addOpts("src", &source_path,
                       "Source path",
                       cat_compile_options, ".", "i");
        parser.addOpts("out", &out_path,
                       "Output path",
                       cat_compile_options, "main.o", "o");
        parser.addOpts("out-ir", &out_with_ir,
                       "Output IR code, enum of {, json, yml, fmt, binary, console}",
                       cat_compile_options, "", "f");
        parser.addOpts("out-token", &out_with_token,
                       "Output tokens, enum of {, json, yml, fmt, binary, console}",
                       cat_compile_options, "", "tf");
    }

    ~CompilerOptions() {
        for (auto &file : files) {
            delete file;
        }
    }


    std::vector<std::fstream *> files;

    std::istream &get_source() {
        if (source_path == "stdin") {
            return std::cin;
        } else {
            auto fs = new std::fstream(source_path);
            if (fs->fail()) {
                delete fs;
                std::cout << strerror(errno) << ": " << source_path;
                exit(errno);
            }

            files.push_back(fs);
            return *fs;
        }
    }
};

template<typename Lexer, typename Parser>
struct Compiler {
    const CompilerOptions &options;
    std::function<void(int code)> _exit;
    LexerProxy<Lexer> lexer;
    ParserProxy<Parser> parser;

    Compiler(
            LexerProxy<Lexer> lexer, ParserProxy<Parser> parser,
            const CompilerOptions &options, std::function<void(int)> _exit)
            :
            lexer(std::move(lexer)), parser(std::move(parser)),
            options(options), _exit(std::move(_exit)) {}

    void work(int &argc, const char *argv[]) {
        reset();
        work_help(argc, argv);
        work_version(argc, argv);
        work_out_tokens(argc, argv);
        work_compile(argc, argv);
    }

    void reset() {
        exited = false;
    }

private:
    void work_help(int, const char *argv[]) {
        if (exited) return;
        if (options.help) {
            std::cout << "usage: " << basename(argv[0]) << " [--version] [-h | --help]\n\n";

            std::cout << dep::print_helps();
            exit(0);
        }
    }

    void work_version(int, const char *[]) {
        if (exited) return;
        if (options.version) {
            std::cout << "Pascal-S Compiler, version " VERSION "\n";
            exit(0);
        }
    }

    void work_out_tokens(int, const char *[]) {
        if (exited) return;
        if (!options.out_with_token.empty()) {
            OStreamProxy<std::ostream> os(std::cout);
            feature::output_tokens(lexer.get_all_tokens(), os);
            lexer.reset_cursor();
        }
    }

    void work_compile(int argc, const char *argv[]) {
        if (exited) return;

        CompilerTargetTask task;

        task.target = options.out_path;
        task.source = parser.parse();

        task.out_ir = !options.out_with_ir.empty();

        _exit(target_compile(argc, argv, &task));
    }

    void exit(int code) {
        exited = true;
        _exit(code);
    }

    bool exited = false;
};

int main(int argc, const char *argv[]) {

    CompilerOptions options(dep::_global);
    dep::parseOpts(argc, argv);

    FullInMemoryLexer lexer(&options.get_source(), &std::cout);
    LexerProxy<FullInMemoryLexer> lexer_proxy(lexer);
    Parser<FullInMemoryLexer> parser(lexer_proxy);
    ParserProxy<Parser<FullInMemoryLexer>> parser_proxy(parser);

    Compiler<FullInMemoryLexer, Parser<FullInMemoryLexer>> compiler(
            lexer_proxy, parser_proxy,
            options, exit);

    compiler.work(argc, argv);
    return 0;
}
