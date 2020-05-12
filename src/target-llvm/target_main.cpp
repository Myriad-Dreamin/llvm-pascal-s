//
// Created by kamiyoru on 2020/5/9.
//

#include <cstring>
#include <pascal-s/ast.h>
#include <target/llvm.h>
#include <cassert>
#include <iostream>
#include <target/task.h>

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"

const char *pascal_main_function_name = "pascal_s_main_520d4d14";

#define LLVM_X86_64_CPU "x86_64"

[[maybe_unused]] int target_compile(int, const char **, CompilerTargetTask *task) {

    auto builder = LLVMBuilder();

//    llvm::legacy::FunctionPassManager fPassM(&builder.modules);
//    fPassM.add(llvm::createPromoteMemoryToRegisterPass());
//    fPassM.add(llvm::createInstructionCombiningPass());
//    fPassM.add(llvm::createReassociatePass());
//    fPassM.doInitialization();

    auto value = builder.code_gen(task->source);

    assert(value != nullptr);

    if (task->out_ir) {
        builder.modules.dump();
    }
//    llvm::InitializeAllTargetInfos();
//    llvm::InitializeAllTargets();
//    llvm::InitializeAllTargetMCs();
//    llvm::InitializeAllAsmParsers();
//    llvm::InitializeAllAsmPrinters();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetDisassembler();

    auto target_triple = llvm::sys::getDefaultTargetTriple();


    builder.modules.setTargetTriple(target_triple);

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(target_triple, Error);

    if (!Target) {
        std::cerr << Error;
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
        return 1;
    }

    llvm::legacy::PassManager pass;

    auto ft = llvm::CGFT_ObjectFile;
    if (machine->addPassesToEmitFile(pass, dest, nullptr, ft)) {
        std::cerr << "TheTargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(builder.modules);
    dest.flush();

//    std::cout << "Wrote " << task->target << "\n";

    dest.close();

    return 0;
}