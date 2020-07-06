//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>


LLVMBuilder::Value *LLVMBuilder::code_gen_exp_call(const ast::ExpCall *pCall) {
    // get function proto llvm_func
    Function *calleeFunc = modules.getFunction(pCall->fn->content);
    if (!calleeFunc) {
        llvm_pascal_s_report_semantic_error_n(pCall->fn,
                                              fmt::format("callee {} not found", pCall->fn->content));
        return nullptr;
    }
    if (calleeFunc->arg_size() != pCall->params->explist.size()) {
        llvm_pascal_s_report_semantic_error_n(
                pCall->params, fmt::format(
                "callee {} expected args size is {}, but length of expression list is {}",
                pCall->fn->content, calleeFunc->arg_size(), pCall->params->explist.size()));
        return nullptr;
    }
    std::vector<Value *> args;

    bool gen_exp_failed = false;

    // for
    for (int i = 0; i < calleeFunc->arg_size(); i++) {
        auto *argument_proto = calleeFunc->getArg(i);
        Value *argument_value = nullptr;
        if (argument_proto->getType()->isPointerTy()) {
            // args[i] = gen_lvalue(call_stmt.args[i])
            argument_value = get_lvalue_pointer(pCall->params->explist[i]);
        } else {
            // args[i] = gen_rvalue(call_stmt.args[i])
            argument_value = code_gen(pCall->params->explist[i]);
        }
        if (argument_value == nullptr) {
            gen_exp_failed = true;
        } else if (argument_proto->getType() != argument_value->getType()) {
            gen_exp_failed = check_extend_type(pCall->params->explist[i]->visit_pos(), argument_value,
                                               argument_proto->getType());
        }
        args.push_back(argument_value);
    }

    if (gen_exp_failed) {
        return nullptr;
    }

    // out_code(%call_stmt.name = call ret_type @call_stmt.name(args))
    if (calleeFunc->getReturnType()->isVoidTy()) {
        return ir_builder.CreateCall(calleeFunc, args);
    } else {
        return ir_builder.CreateCall(calleeFunc, args, pCall->fn->content);
    }
}
