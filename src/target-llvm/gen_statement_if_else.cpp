//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>


LLVMBuilder::Value *LLVMBuilder::code_gen_if_else_statement(const ast::IfElseStatement *if_else_stmt) {

    // cond_value = gen_exp(if_else_stmt.cond)
    Value *cond = code_gen(if_else_stmt->cond);
    if (!cond) {
        llvm_pascal_s_report_semantic_error_n(if_else_stmt->cond, "gen expression error");
        return nullptr;
    }

    // out_code(cond = cond_value is zero value)
    switch (cond->getType()->getTypeID()) {
        case llvm::Type::TypeID::IntegerTyID:
            cond = ir_builder.CreateICmpNE(
                    cond, llvm::Constant::getIntegerValue(
                            cond->getType(), llvm::APInt(cond->getType()->getIntegerBitWidth(), 0)), "if_cond");
            break;
        case llvm::Type::TypeID::DoubleTyID:
            cond = ir_builder.CreateFCmpONE(
                    cond, llvm::ConstantFP::get(cond->getType(), llvm::APFloat(double(0))), "if_cond");
            break;
        default:
            llvm_pascal_s_report_semantic_error_n(
                    if_else_stmt->cond,
                    fmt::format("could not perform !=(<>) on not boolean condition expression,"
                                "if-exp type = {}", format_type(cond->getType())));
            return nullptr;
    }


    Function *cur_function = ir_builder.GetInsertBlock()->getParent();
    // push then block
    // create basic block then_block
    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(ctx, "then", cur_function);
    // create basic block else_block
    llvm::BasicBlock *else_block = nullptr;
    if (if_else_stmt->else_part) {
        else_block = llvm::BasicBlock::Create(ctx, "else");
    }
    // create basic block merge_block
    llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(ctx, "follow_ie");

    // link before if to -> cond ? then_block : else_block
    // out_code(cond_br cond ? then_block : (else_block ? else_block : merge_block))
    ir_builder.CreateCondBr(cond, then_block, else_block ? else_block : merge_block);

    // already pushed then block
    // build then block
    ir_builder.SetInsertPoint(then_block);
    Value *then_value = code_gen_statement(if_else_stmt->if_part);
    ir_builder.CreateBr(merge_block);
    then_block = ir_builder.GetInsertBlock();

    Value *else_value = nullptr;
    if (if_else_stmt->else_part) {
        // push else block
        cur_function->getBasicBlockList().push_back(else_block);
        // build else block
        ir_builder.SetInsertPoint(else_block);
        else_value = code_gen_statement(if_else_stmt->else_part);
        ir_builder.CreateBr(merge_block);
        else_block = ir_builder.GetInsertBlock();
    }

    // push merge block
    cur_function->getBasicBlockList().push_back(merge_block);
    ir_builder.SetInsertPoint(merge_block);

    if (!(then_value && (!if_else_stmt->else_part || else_value)))
        return nullptr;

    if (else_value && then_value->getType()->getTypeID() == else_value->getType()->getTypeID()) {
        //assuming equal

        llvm::PHINode *merged_value = ir_builder.CreatePHI(
                then_value->getType(), 2, "ieb_eval");

        merged_value->addIncoming(then_value, then_block);
        merged_value->addIncoming(else_value, else_block);
        return merged_value;
    }

    return else_value ? llvm::UndefValue::get(llvm::Type::getVoidTy(ctx)) : then_value;
}
