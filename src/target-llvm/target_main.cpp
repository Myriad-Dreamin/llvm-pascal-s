//
// Created by kamiyoru on 2020/5/9.
//

#include <target/llvm.h>
#include <iostream>
#include <target/task.h>
#include "pascal-s/features.h"
#include <pascal-s/errno.h>

#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#ifdef PASCAL_S_LLVM_COMPILER_PASS_OPTIMIZE
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#endif

const char *pascal_main_function_name = "pascal_s_main_520d4d14";

#define LLVM_X86_64_CPU "x86_64"

template<>
struct ErrorProxy<PascalSSemanticError *> {
    DefaultProxyConstructor(ErrorProxy, PascalSSemanticError*, err)

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

[[maybe_unused]] int target_compile(int, const char **, CompilerTargetTask *task) {

    auto builder = LLVMBuilder();

    llvm::legacy::FunctionPassManager fPassM(&builder.modules);

#ifdef PASCAL_S_LLVM_COMPILER_PASS_OPTIMIZE
    builder.fn_pass_manager.add(llvm::createPromoteMemoryToRegisterPass());
    builder.fn_pass_manager.add(llvm::createInstructionCombiningPass());
    builder.fn_pass_manager.add(llvm::createReassociatePass());
#endif

    builder.doInitialization();

    auto value = builder.code_gen(task->source);

    if (!builder.warnings.empty()) {
        for (auto e : builder.warnings) {
            feature::format_line_column_error(
                    task->f, ErrorProxy<PascalSSemanticError *>(e), task->os, task->file_path);
        }
    }

    if (!builder.errors.empty()) {
        for (auto e : builder.errors) {
            feature::format_line_column_error(
                    task->f, ErrorProxy<PascalSSemanticError *>(e), task->os, task->file_path);
        }
        return pascal_s::ProgramErrorCode::BuildFailed;
    }

    if (value == nullptr) {
        return pascal_s::ProgramErrorCode::BuilderCouldNotGenTarget;
    }

    if (task->out_ir) {
        builder.modules.dump();
    }

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetDisassembler();

    auto target_triple = llvm::sys::getDefaultTargetTriple();


    builder.modules.setTargetTriple(target_triple);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(target_triple, Error);

    if (!Target) {
        std::cerr << "llvm look up target error: " << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();

    auto machine =
            Target->createTargetMachine(target_triple, CPU, Features, opt, RM);
    builder.modules.setDataLayout(machine->createDataLayout());

    std::error_code EC;
    llvm::raw_fd_ostream dest(task->target, EC, llvm::sys::fs::OF_None);
    if (EC) {
        std::cerr << "Could not open file: " << EC.message();
        return pascal_s::ProgramErrorCode::CantOpenTargetFile;
    }

    llvm::legacy::PassManager pass;

    auto ft = llvm::CGFT_ObjectFile;
    if (machine->addPassesToEmitFile(pass, dest, nullptr, ft)) {
        std::cerr << "TheTargetMachine can't emit a file of this type";
        return pascal_s::ProgramErrorCode::TargetFileEmitFailed;
    }

    pass.run(builder.modules);
    dest.flush();

//    std::cout << "Wrote " << task->target << "\n";

    dest.close();

    return pascal_s::ProgramErrorCode::CodeOK;
}