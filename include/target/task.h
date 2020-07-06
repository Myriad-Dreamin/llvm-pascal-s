//
// Created by kamiyoru on 2020/5/9.
//

#ifndef PASCAL_S_TARGET_TASK_H
#define PASCAL_S_TARGET_TASK_H

#include <string>
#include <pascal-s/llvm-ast.h>
#include <pascal-s/interface.h>
#include <pascal-s/lib/stream_file.h>

template<typename File, typename OStream>
struct CompilerTargetTaskT {
    FileProxy<File> f;
    WriterProxy<OStream> &os;
    const char *file_path = nullptr;

    ast::Node *source = nullptr;
    std::string target;
    bool out_ir = false;
};

using CompilerTargetTask = CompilerTargetTaskT<pascal_s::CPPStreamFile, std::ostream>;

#endif //PASCAL_S_TARGET_TASK_H
