//
// Created by kamiyoru on 2020/7/3.
//

#ifndef PASCAL_S_COMPILER_OPTION_H
#define PASCAL_S_COMPILER_OPTION_H

#include <fstream>
#include <iostream>
#include <dep/opt.h>

static const char *cat_default = "";
static const char *cat_compile_options = "compile options";

struct CompilerOptions {
    bool help = false;
    bool version = false;
    bool env = false;
    std::string wk_dir;
    std::string source_path;
    std::string out_path;
    std::string out_with_ir;
    std::string out_with_ast;
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
        parser.addOpts("out-ast", &out_with_ast,
                       "Output asts, enum of {, json, yml, fmt, binary, console}",
                       cat_compile_options, "", "af");
    }

    ~CompilerOptions() {
        for (auto &file : files) {
            delete file;
        }
    }

    std::vector<std::fstream *> files;

    std::fstream *source_file = nullptr;

    std::istream &get_source() {
        if (source_path == "stdin") {
            return std::cin;
        } else {
            if (source_file != nullptr) {
                return *source_file;
            }

            auto fs = new std::fstream(source_path);
            if (fs->fail()) {
                delete fs;
                std::cerr << "Open Source File Error: " << strerror(errno) << ": " << source_path;
                exit(errno);
            }

            files.push_back(fs);
            source_file = fs;
            return *fs;
        }
    }
};

#endif //PASCAL_S_COMPILER_OPTION_H
