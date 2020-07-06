//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>


LLVMBuilder::Value *LLVMBuilder::code_gen_ident(const ast::Ident *pIdent) {

    for (auto resolving_ctx = scope_stack;
         resolving_ctx; resolving_ctx = resolving_ctx->last) {
        auto &value_ctx = resolving_ctx->ctx;
        // if is a variable
        if (value_ctx->count(pIdent->ident->content)) {
            // out_code ( load_tmp = load from %ident.name )
            // return load_tmp
            return ir_builder.CreateLoad(value_ctx->at(pIdent->ident->content), "load_tmp");
        }
        // if is a constant
        auto &const_ctx = resolving_ctx->const_ctx;
        if (const_ctx->count(pIdent->ident->content)) {
            // return const_value
            return const_ctx->at(pIdent->ident->content);
        }
    }
    // get function proto llvm_func
    Function *calleeFunc = modules.getFunction(pIdent->ident->content);
    if (!calleeFunc) {
        llvm_pascal_s_report_semantic_error_n(pIdent, "neither variable nor function");
        return nullptr;
    }

    // if ident is a function

    if (calleeFunc->arg_size() != 0) {
        llvm_pascal_s_report_semantic_error_n(pIdent, fmt::format(
                "callee {} expected args size is {}, but length of expression list is 0",
                pIdent->ident->content, calleeFunc->arg_size()));
        return nullptr;
    }

    // out_code(%ident.content.name = call ret_type @ident.content.name())
    // return gen_call(ident)
    if (calleeFunc->getReturnType()->isVoidTy()) {
        return ir_builder.CreateCall(calleeFunc, {});
    } else {
        return ir_builder.CreateCall(calleeFunc, {}, pIdent->ident->content);
    }
}

LLVMBuilder::Value *LLVMBuilder::code_gen_variable(const ast::Variable *node) {
    // ptr = get_lvalue_pointer(var)
    auto ptr = get_lvalue_pointer(node);
    if (ptr == nullptr) {
        llvm_pascal_s_report_semantic_error_n(node, "cant get lvalue of node");
        return nullptr;
    }

    // out_code ( var_load = load from ptr )
    // return var_load
    return ir_builder.CreateLoad(ptr, "var_load");
}

LLVMBuilder::Value *LLVMBuilder::code_gen_exp_constant_boolean(const ast::ExpConstantBoolean *pBoolean) {
    return llvm::Constant::getIntegerValue(
            llvm::Type::getInt1Ty(ctx), llvm::APInt(1, pBoolean->value->attr));
}

LLVMBuilder::Value *LLVMBuilder::code_gen_exp_constant_integer(const ast::ExpConstantInteger *pInteger) {
    return llvm::Constant::getIntegerValue(
            llvm::Type::getInt64Ty(ctx), llvm::APInt(64, pInteger->value->attr));
}

LLVMBuilder::Value *LLVMBuilder::code_gen_exp_constant_char(const ast::ExpConstantChar *pChar) {
    return llvm::Constant::getIntegerValue(
            llvm::Type::getInt8Ty(ctx), llvm::APInt(8, pChar->value->attr));
}

LLVMBuilder::Value *LLVMBuilder::code_gen_exp_constant_real(const ast::ExpConstantReal *pReal) {
    return llvm::ConstantFP::get(
            llvm::Type::getDoubleTy(ctx), llvm::APFloat(pReal->value->attr));
}

LLVMBuilder::Value *LLVMBuilder::code_gen_exp_constant_string(const ast::ExpConstantString *cs) {
    llvm_pascal_s_report_semantic_error_n(cs, "constant string is not available now");
    assert(false);
    return nullptr;
}
