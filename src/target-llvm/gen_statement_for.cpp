//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>

LLVMBuilder::Value *LLVMBuilder::code_gen_for_statement(const ast::ForStatement *for_stmt) {

    // from_value = gen_exp(for_stmt.from_exp)
    Value *from_value = code_gen(for_stmt->from_exp);
    // to_value = gen_exp(for_stmt.to_exp)
    Value *to_value = code_gen(for_stmt->to_exp);

    if (from_value == nullptr || to_value == nullptr) {
        return nullptr;
    }

    if (from_value->getType() != to_value->getType() && !check_extend_type(
            for_stmt->from_exp, from_value, to_value, true)) {
        return nullptr;
    }

    // out_code( define variable from_value.type: %for_stmt.loop_var.name )
    Function *cur_function = ir_builder.GetInsertBlock()->getParent();
    llvm::IRBuilder<> dfn_block(&cur_function->getEntryBlock(),
                                cur_function->getEntryBlock().begin());
    llvm::AllocaInst *loop_var = dfn_block.CreateAlloca(from_value->getType(), nullptr,
                                                        for_stmt->loop_var->content);

    // out_code( define constant from_value.type: %step_value = 1 )
    Value *step_value;
    switch (from_value->getType()->getTypeID()) {
        case llvm::Type::IntegerTyID:
            step_value = llvm::Constant::getIntegerValue(
                    from_value->getType(), llvm::APInt(from_value->getType()->getIntegerBitWidth(), 1));
            break;
        case llvm::Type::DoubleTyID:
            step_value = llvm::ConstantFP::get(
                    from_value->getType(), llvm::APFloat(1.0));
            break;
        default:
            llvm_pascal_s_report_semantic_error_n(for_stmt->loop_var,
                                                  fmt::format("iter type invalid error, from-to-exp type = {}",
                                                              format_type(from_value->getType())));
            return nullptr;
    }

    // cover loop_var local name
    llvm::Value *old_var = nullptr;
    if (scope_stack->ctx->count(for_stmt->loop_var->content)) {
        old_var = scope_stack->ctx->at(for_stmt->loop_var->content);
    }
    (*scope_stack->ctx)[for_stmt->loop_var->content] = loop_var;

    // out_code( store from_value into %for_stmt.loop_var.name )
    auto si = ir_builder.CreateStore(from_value, loop_var);


    // create basic block loop_block
    llvm::BasicBlock *loop_block = llvm::BasicBlock::Create(ctx, "loop", cur_function);
    // create basic block loop_after_block
    llvm::BasicBlock *loop_after_block =
            llvm::BasicBlock::Create(ctx, "follow_lp");

    Value *first_loop_cond = nullptr;
    // out_code(loop_cond = from_value <= to_value)
    switch (from_value->getType()->getTypeID()) {
        case llvm::Type::IntegerTyID:
            // todo: signed/unsigned integer type feature
            first_loop_cond = ir_builder.CreateICmpSLE(
                    from_value, to_value, "loop_cond");
            break;
        case llvm::Type::DoubleTyID:
            // todo: ordered or unordered
            first_loop_cond = ir_builder.CreateFCmpOLE(
                    from_value, to_value, "loop_cond");
            break;
        default:
            llvm_pascal_s_report_semantic_error_n(
                    for_stmt->from_exp,
                    fmt::format("could not perform binary calc <= on lhs and rhs, lhs type = {}, rhs type = {}",
                                format_type(from_value->getType()), format_type(to_value->getType())));
            return nullptr;
    }

    // out_code(cond_br loop_cond ? loop_block : loop_after_block)
    ir_builder.CreateCondBr(first_loop_cond, loop_block, loop_after_block);

    // out_code('loop:') // loop_block label
    ir_builder.SetInsertPoint(loop_block);
    // for_value = gen_statement(for_stmt.for_stmt)
    Value *loop_body = code_gen_statement(for_stmt->for_stmt);
    if (si == nullptr || loop_body == nullptr)
        return nullptr;

    // out_code(cur_value = load from_type.type from %for_stmt.loop_var.name)
    Value *cur_value = ir_builder.CreateLoad(loop_var, for_stmt->loop_var->content);
    // out_code(next_tmp = %step_value + %cur_value)
    // out_code(store next_tmp into %for_stmt.loop_var.name)
    switch (from_value->getType()->getTypeID()) {
        case llvm::Type::IntegerTyID:
            si = ir_builder.CreateStore(
                    ir_builder.CreateAdd(cur_value, step_value, "next_tmp"), loop_var);
            break;
        case llvm::Type::DoubleTyID:
            si = ir_builder.CreateStore(
                    ir_builder.CreateFAdd(cur_value, step_value, "next_tmp_d"), loop_var);
            break;
        default:
            llvm_pascal_s_report_semantic_error_n(
                    for_stmt->loop_var,
                    fmt::format("could not perform binary calc + on lhs and rhs, lhs type = {}, rhs type = {}",
                                format_type(cur_value->getType()), format_type(step_value->getType())));
            return nullptr;
    }
    if (si == nullptr)
        return nullptr;

    // out_code(loop_cond = %next_tmp <= to_value)
    Value *loop_cond = ir_builder.CreateLoad(loop_var);
    if (!loop_cond) {
        llvm_pascal_s_report_semantic_error_n(for_stmt->loop_var, "internal load invalid error");
        return nullptr;
    }
    switch (from_value->getType()->getTypeID()) {
        case llvm::Type::IntegerTyID:
            // todo: signed/unsigned integer type feature
            loop_cond = ir_builder.CreateICmpSLE(
                    loop_cond, to_value, "loop_cond");
            break;
        case llvm::Type::DoubleTyID:
            // todo: ordered or unordered
            loop_cond = ir_builder.CreateFCmpOLE(
                    loop_cond, to_value, "loop_cond");
            break;
        default:
            llvm_pascal_s_report_semantic_error_n(
                    for_stmt->loop_var,
                    fmt::format("could not perform binary calc <= on lhs and rhs, lhs type = {}, rhs type = {}",
                                format_type(loop_cond->getType()), format_type(to_value->getType())));
            return nullptr;
    }

    // out_code(cond_br loop_cond ? loop_block : loop_after_block)
    ir_builder.CreateCondBr(loop_cond, loop_block, loop_after_block);

    // out_code('follow_lp:') // loop_after_block label
    cur_function->getBasicBlockList().push_back(loop_after_block);
    ir_builder.SetInsertPoint(loop_after_block);

    // recover (restore) loop_var local name if it is ever shadow
    if (old_var != nullptr) {
        (*scope_stack->ctx)[for_stmt->loop_var->content] = loop_var;
    } else {
        scope_stack->ctx->erase(for_stmt->loop_var->content);
    }

    return loop_body;
}