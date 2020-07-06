//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>


LLVMBuilder::Value *LLVMBuilder::code_gen_statement_block(const ast::StatementList *pBlock) {
    Value *block_value = nullptr;
    bool collapsed = false;

    for (auto stmt : pBlock->stmts) {

        // block_value = gen_statement(statement)
        block_value = code_gen_statement(stmt);
        if (block_value == nullptr) {
            collapsed = true;
        }
    }

    // return nullptr if any statement generate failed else block_value
    return collapsed ? nullptr : block_value;
}