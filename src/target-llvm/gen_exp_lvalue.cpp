//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>


llvm::Value *LLVMBuilder::get_lvalue_pointer(const ast::Exp *lvalue) {

    const char *content;
    const ast::ExpressionList *exp_list = nullptr;
    switch (lvalue->type) {
        case ast::Type::Ident:
            content = reinterpret_cast<const ast::Ident *>(lvalue)->ident->content;
            break;
        case ast::Type::Variable:
            content = reinterpret_cast<const ast::Variable *>(lvalue)->id->content;
            exp_list = reinterpret_cast<const ast::Variable *>(lvalue)->id_var;
            break;
        default:
            llvm_pascal_s_report_semantic_error_n(lvalue, "not ident or variable, should not have lvalue");
            return nullptr;
    }

    std::vector<Value *> offset;

    for (auto resolving_ctx = scope_stack;
         resolving_ctx; resolving_ctx = resolving_ctx->last) {
        auto &value_ctx = resolving_ctx->ctx;
        if (value_ctx->count(content)) {

            // pointer stored in the this.ctx
            auto ptr = (*value_ctx).at(content);

            // if lvalue is a array
            if (resolving_ctx->array_infos->count(content)) {
                if (exp_list != nullptr) {
                    offset.push_back(llvm::ConstantInt::get(ctx, llvm::APInt(64, 0, true)));
                    // offset = gen_offset(lvalue.explist)
                    code_gen_offset(offset, resolving_ctx->array_infos->at(content), exp_list);
                    if (offset.empty()) {
                        return nullptr;
                    } else {
                        if (ptr->getType()->getPointerElementType()->isArrayTy()) {
                            // 先取地址得到数组类型 [ lvalue.array_size x i64]，再取偏移量offset
                            // 相当于lvalue在C代码中的类型相当于array_type **lvalue
                            // 则此指令完成 gep_tmp = lvalue[0][offset]
                            return ir_builder.CreateGEP(ptr, offset, "gep_tmp");
                        } else {
                            return ir_builder.CreateGEP(ptr, offset[1], "gep_tmp");
                        }
                    }
                } else {
                    llvm_pascal_s_report_semantic_error_n(
                            lvalue, fmt::format("lvalue is a array, but index operator [] is missing"));
                    return nullptr;
                }
            }
            // if lvalue is a variable
            // else { do nothing }

            return ptr;
        }
        auto &const_ctx = resolving_ctx->const_ctx;
        if (const_ctx->count(content)) {
            llvm_pascal_s_report_semantic_error_n(lvalue, "could not move const value to lvalue");
            return nullptr;
        }
    }
    llvm_pascal_s_report_semantic_error_n(lvalue, "variable not found");
    return nullptr;
}

