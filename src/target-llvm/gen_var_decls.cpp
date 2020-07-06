//
// Created by kamiyoru on 2020/6/30.
//

#include <target/llvm.h>
#include <fmt/core.h>

void LLVMBuilder::insert_var_decls(LLVMBuilder::Function *cur_func,
                                   std::map<std::string, pascal_s::ArrayInfo *> &array_infos,
                                   std::map<std::string, llvm::Value *> &map,
                                   const ast::VarDecls *pDecls, bool on_stack) {
    if (pDecls != nullptr) {
        llvm::IRBuilder<> dfn_block(&cur_func->getEntryBlock(),
                                    cur_func->getEntryBlock().begin());

        for (auto decl : pDecls->decls) {
            llvm::Type *llvm_type = create_type(decl->type_spec);
            if (llvm_type == nullptr) {
                continue;
            }

            if (decl->type_spec->type == ast::Type::ArrayTypeSpec) {

                for (auto ident : decl->idents->idents) {
                    if (map.count(ident->content)) {
                        llvm_pascal_s_report_semantic_error_n(
                                ident, fmt::format("ident redeclared"));
                        continue;
                    }
                    if (on_stack) {
                        map[ident->content] = dfn_block.CreateAlloca(
                                llvm_type->getArrayElementType(),
                                llvm::Constant::getIntegerValue(
                                        llvm::IntegerType::getInt64Ty(ctx),
                                        llvm::APInt(64, calc_periods_size(
                                                reinterpret_cast<const ast::ArrayTypeSpec *>(decl->type_spec)))),
                                ident->content);
                    } else {
                        map[ident->content] = new llvm::GlobalVariable(
                                modules, llvm_type, false,
                                llvm::GlobalVariable::LinkageTypes::InternalLinkage,
                                llvm::ConstantArray::getNullValue(llvm_type), ident->content);
                    }

                    array_infos[ident->content] = new pascal_s::ArrayInfo{
                            reinterpret_cast<const ast::ArrayTypeSpec *>(decl->type_spec)};
                }
            } else {
                for (auto ident : decl->idents->idents) {
                    if (map.count(ident->content)) {
                        llvm_pascal_s_report_semantic_error_n(
                                ident, fmt::format("ident redeclared"));
                        continue;
                    }

                    if (on_stack) {
                        map[ident->content] = dfn_block.CreateAlloca(llvm_type, nullptr, ident->content);
                    } else {
                        if (llvm_type->isIntegerTy()) {
                            map[ident->content] = new llvm::GlobalVariable(
                                    modules, llvm_type, false,
                                    llvm::GlobalVariable::LinkageTypes::InternalLinkage,
                                    llvm::Constant::getIntegerValue(llvm_type,
                                                                    llvm::APInt(llvm_type->getIntegerBitWidth(), 0)),
                                    ident->content);
                        } else if (llvm_type->isDoubleTy()) {
                            map[ident->content] = new llvm::GlobalVariable(
                                    modules, llvm_type, false,
                                    llvm::GlobalVariable::LinkageTypes::InternalLinkage,
                                    llvm::ConstantFP::get(llvm_type, llvm::APFloat(.0)), ident->content);
                        } else {
                            llvm_pascal_s_report_semantic_error_n(
                                    ident, fmt::format("ident with type spec {} could not convert to a llvm type",
                                                       static_cast<uint16_t>(decl->type_spec->type)));
                        }
                    }
                }
            }
        }
    }
}
