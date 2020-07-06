//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>

LLVMBuilder::Value *LLVMBuilder::code_gen_exp_assign(const ast::ExpAssign *pAssign) {

    // rhs = gen_exp(assign_exp.rhs)
    auto rhs = code_gen(pAssign->rhs);
    // lvalue = gen_lvalue(assign_exp.lhs)
    auto ptr = get_lvalue_pointer(pAssign->lhs);
    if (ptr == nullptr || rhs == nullptr) {
        return nullptr;
    }

    // out_code( store rhs into lvalue )
    // assign_lvalue only helps check if it is valid operation
    return assign_lvalue(pAssign->lhs->visit_pos(), ptr, rhs);
}
