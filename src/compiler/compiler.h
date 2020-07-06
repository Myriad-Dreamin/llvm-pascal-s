//
// Created by kamiyoru on 2020/7/3.
//

#ifndef PASCAL_S_COMPILER_H
#define PASCAL_S_COMPILER_H

#include "compiler_option.h"
#include <pascal-s/features.h>
#include <pascal-s/lib/stream_file.h>
#include <utility>
#include <dep/stl.h>
#include <target/task.h>
#include <functional>
#include <pascal-s/errno.h>

string_view_return basename(string_view s) {
    for (size_t i = s.length() - 1; i >= 0; i--) {
        if (s[i] == '\\' || s[i] == '/') {
            return s.substr(i + 1);
        }
    }
    return s;
}

extern int target_compile(int argc, const char **pString, CompilerTargetTask *task);

template<>
struct ErrorProxy<const PascalSError *> {
    DefaultProxyConstructor(ErrorProxy, PascalSError*, err)

    [[maybe_unused]] [[nodiscard]] pascal_s::line_t visit_line() const {
        return err->line;
    }

    [[maybe_unused]] [[nodiscard]] pascal_s::column_t visit_column() const {
        return err->column;
    }

    [[maybe_unused]] [[nodiscard]] pascal_s::length_t visit_length() const {
        return err->length;
    }

    [[maybe_unused]] [[nodiscard]] pascal_s::offset_t visit_offset() const {
        return err->offset;
    }

    // 如果没有hint，为nullptr
    [[maybe_unused]] [[nodiscard]] const char *visit_hint() const {
        return err->msg.c_str();
    }
};

template<typename Lexer, typename Parser>
struct Compiler {
    CompilerOptions &options;
    std::function<void(int code)> _exit;
    LexerProxy<Lexer> lexer;
    ParserProxy<Parser> parser;

    Compiler(
            LexerProxy<Lexer> lexer,
            ParserProxy<Parser> parser,
            CompilerOptions &options, std::function<void(int)> _exit)
            :
            lexer(std::move(lexer)), parser(std::move(parser)),
            options(options), _exit(std::move(_exit)) {}

    ~Compiler() {
        if (_cached_ast != nullptr) {
            ast::deleteAST(_cached_ast);
            _cached_ast = nullptr;
        }
    }

    int work(int &argc, const char *argv[]) {
        reset();
        work_help(argc, argv);
        work_version(argc, argv);
        work_out_tokens(argc, argv);
        work_out_ast(argc, argv);
        work_compile(argc, argv);

        return _code;
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
            exit(pascal_s::ProgramErrorCode::CodeOK);
        }
    }

    void work_version(int, const char *[]) {
        if (exited) return;
        if (options.version) {
            std::cout << "Pascal-S Compiler, version " VERSION "\n";
            exit(pascal_s::ProgramErrorCode::CodeOK);
        }
    }

    void work_out_tokens(int, const char *[]) {
        if (exited) return;
        if (!options.out_with_token.empty()) {
            WriterProxy<std::ostream> os(std::cout);
            feature::output_tokens(lexer.get_all_tokens(), os);
            lexer.reset_cursor();
            fflush(stdout);
        }
    }

    void work_out_ast(int, const char *[]) {
        if (exited) return;
        if (!options.out_with_ast.empty()) {
            ast::printAST(get_ast());
            fflush(stdout);
        }
    }

    void work_compile(int argc, const char *argv[]) {
        if (exited) return;
        pascal_s::CPPStreamFile fin(options.get_source());
        FileProxy<pascal_s::CPPStreamFile> fp(fin);
        WriterProxy<std::ostream> os(std::cerr);

        CompilerTargetTask task{fp, os};

        task.file_path = options.source_path.c_str();
        task.target = options.out_path;
        task.out_ir = !options.out_with_ir.empty();

        task.source = get_ast();

        if (lexer.has_error() || parser.has_error()) {
            exit(pascal_s::ProgramErrorCode::ParseFailed);
        }

        if (task.source == nullptr) {
            exit(pascal_s::ProgramErrorCode::ParserReturnNullNode);
        }

        fflush(stdout);
        fflush(stderr);
        exit(target_compile(argc, argv, &task));
    }


    ast::Node *_cached_ast = nullptr;

    ast::Node *get_ast() {
        if (_cached_ast) return _cached_ast;

        _cached_ast = parser.parse();

        if (lexer.has_error()) {

            pascal_s::CPPStreamFile fin(options.get_source());
            FileProxy<pascal_s::CPPStreamFile> fp(fin);
            WriterProxy<std::ostream> os(std::cerr);

            for (auto e : lexer.get_all_errors()) {
                feature::format_line_column_error(fp, ErrorProxy<ErrorToken>(*e), os, options.source_path.c_str());
            }
        }

        if (parser.has_error()) {

            pascal_s::CPPStreamFile fin(options.get_source());
            FileProxy<pascal_s::CPPStreamFile> fp(fin);
            WriterProxy<std::ostream> os(std::cerr);

            for (auto e : parser.get_all_errors()) {
                feature::format_line_column_error(fp, ErrorProxy<const PascalSError *>(e), os,
                                                  options.source_path.c_str());
            }
        }

        return _cached_ast;
    }

    void exit(int code) {
        exited = true;
        if (_cached_ast != nullptr) {
            ast::deleteAST(_cached_ast);
            _cached_ast = nullptr;
        }
        _exit(_code = code);
    }

    bool exited = false;
    int _code = pascal_s::ProgramErrorCode::CodeOK;
};

#endif //PASCAL_S_COMPILER_H
