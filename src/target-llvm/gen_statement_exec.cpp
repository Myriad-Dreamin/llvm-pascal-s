//
// Created by kamiyoru on 2020/7/4.
//

#include <target/llvm.h>


LLVMBuilder::Value *LLVMBuilder::code_gen_exec_statement(const ast::ExecStatement *pStatement) {
    // return gen_exp(exec_stmt.exp)
    return code_gen(pStatement->exp);
}
