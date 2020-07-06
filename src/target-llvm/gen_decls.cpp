//
// Created by kamiyoru on 2020/5/25.
//

#include <target/llvm.h>

void LLVMBuilder::insert_param_decls(const pascal_s::Pos *pos, LLVMBuilder::Function *fn,
                                     std::map<std::string, llvm::Value *> &map,
                                     std::map<std::string, llvm::Value *> &const_map) {
    for (size_t i = 0; i < fn->arg_size(); i++) {
        auto arg = fn->getArg(i);
        if (map.count(arg->getName().str()) || const_map.count(arg->getName().str())) {
            llvm_pascal_s_report_semantic_error(pos, "param ident redeclared");
            continue;
        }

        if (arg->getType()->isPointerTy()) {
            map[arg->getName().str()] = arg;// llvm::dyn_cast<llvm::AllocaInst>(arg);
        } else {
            const_map[arg->getName().str()] = arg;
        }
    }
}


void LLVMBuilder::insert_const_decls(std::map<std::string, llvm::Value *> &map, const ast::ConstDecls *pDecls) {
    if (pDecls != nullptr) {
        for (auto decl : pDecls->decls) {
            if (map.count(decl->ident->content)) {
                llvm_pascal_s_report_semantic_error(decl->ident, "const ident redeclared");
                continue;
            }

            map[decl->ident->content] = code_gen(decl->rhs);
        }
    }
}
