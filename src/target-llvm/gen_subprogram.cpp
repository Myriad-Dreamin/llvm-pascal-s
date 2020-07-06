//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>

LLVMBuilder::Function *LLVMBuilder::code_gen_subprogram(const ast::Subprogram *pSubprogram) {
    Function *fn = modules.getFunction(pSubprogram->head->name->content);

    // check and gen function proto llvm_func
    if (!fn) {
        llvm::Type *llvm_ret_type = nullptr;
        if (pSubprogram->head->ret_type == nullptr) {
            llvm_ret_type = llvm::Type::getVoidTy(ctx);
        } else {
            llvm_ret_type = create_type(pSubprogram->head->ret_type);
        }

        std::vector<llvm::Type *> args_proto;

        if (pSubprogram->head->decls != nullptr) {
            int args_proto_size = 0;

            for (auto arg_spec : pSubprogram->head->decls->params) {
                args_proto_size += arg_spec->id_list->idents.size();
            }
            args_proto.reserve(args_proto_size);

            for (auto arg_spec : pSubprogram->head->decls->params) {
                llvm::Type *llvm_arg_type = create_type(arg_spec->spec);

                if (arg_spec->keyword_var != nullptr) {
                    llvm_arg_type = llvm_arg_type->getPointerTo();
                }

                for (auto ident: arg_spec->id_list->idents) {
                    args_proto.push_back(llvm_arg_type);
                }
            }
        }

        auto *prototype = llvm::FunctionType::get(
                llvm_ret_type, args_proto, false);

        fn = Function::Create(prototype, Function::ExternalLinkage,
                              pSubprogram->head->name->content, modules);

        if (pSubprogram->head->decls != nullptr) {
            int arg_cursor = 0;
            for (auto arg_spec : pSubprogram->head->decls->params) {
                for (auto ident: arg_spec->id_list->idents) {
                    fn->getArg(arg_cursor++)->setName(ident->content);
                }
            }
        }
    } else {
        // repeated implementation is not allowed
        if (pSubprogram->body != nullptr) {
            llvm_pascal_s_report_semantic_error_n(
                    pSubprogram->head,
                    "subprogram has multiple implementation");
            return nullptr;
        }

        // repeated definition is allowed, so just check signature

        // check void param signature
        if (pSubprogram->head->decls == nullptr) {
            llvm_pascal_s_report_semantic_error_n(
                    pSubprogram->head,
                    "subprogram is already defined, but prototype are different (want empty param list)");
            return nullptr;
        }

        // check signature
        int arg_cursor = 0, arg_size = fn->arg_size();
        for (auto arg_spec : pSubprogram->head->decls->params) {
            llvm::Type *llvm_arg_type = create_type(arg_spec->spec);

            if (arg_spec->keyword_var != nullptr) {
                llvm_arg_type = llvm_arg_type->getPointerTo();
            }

            for (auto ident: arg_spec->id_list->idents) {
                if (arg_cursor == arg_size) {
                    llvm_pascal_s_report_semantic_error_n(
                            ident, fmt::format("subprogram is already defined, but prototype are different"
                                               " (old definition has {} params, less than new definition)",
                                               arg_size));
                    return nullptr;
                }
                auto a = fn->getArg(arg_cursor++);
                if (llvm_arg_type != a->getType()) {
                    llvm_pascal_s_report_semantic_error_n(
                            ident, fmt::format("the i-th param-proto type is incompatible, "
                                               "old type = {}, new type = {}",
                                               format_type(a->getType()), format_type(llvm_arg_type)));
                    return nullptr;
                }
            }
        }

        return fn;
    }

    // function_entry = out_code('entry:')
    llvm::BasicBlock *body = llvm::BasicBlock::Create(ctx, "entry", fn);
    ir_builder.SetInsertPoint(body);

    // create local const map this.const_ctx
    std::map<std::string, Value *> program_const_this;
    // insert subprogram.const_decls to this.const_ctx
    insert_const_decls(program_const_this, pSubprogram->body->const_decls);
    // create local variable map this.ctx
    std::map<std::string, pascal_s::ArrayInfo *> program_array_infos;
    std::map<std::string, llvm::Value *> program_this;
    // insert subprogram.var_decls to this.ctx
    insert_var_decls(fn, program_array_infos, program_this, pSubprogram->body->var_decls, true);

    // check this.const_ctx and this.ctx to avoid conflict
    if (pSubprogram->body->const_decls != nullptr) {
        for (auto d : pSubprogram->body->const_decls->decls) {
            if (program_this.count(d->ident->content)) {
                llvm_pascal_s_report_semantic_error_n(
                        d->ident, fmt::format("ident redeclared"));
            }
        }
    }

    // push 'this' into scope stack
    auto link = LinkedContext{scope_stack, &program_array_infos, &program_this, &program_const_this};
    scope_stack = &link;

//    if (pSubprogram->subbody->subprogram != nullptr) {
//        for (auto fn_decl : pSubprogram->subbody->subprogram->subprogram) {
//            if (program_this.count(fn_decl->head->name->content) ||
//                program_const_this.count(fn_decl->head->name->content)) {
//                llvm_pascal_s_report_semantic_error_n(
//                        fn_decl->head->name,
//                        fmt::format("function redeclared, another variable is in this context"));
//            }
//            code_gen_subprogram(fn_decl);
//        }
//    }

    // insert subprogram.param_decls to this.ctx and this.const_ctx
    if (pSubprogram->head->decls) {
        insert_param_decls(pSubprogram->head->decls->visit_pos(), fn, program_this, program_const_this);
    }

    // out_code( define variable ret_type: %subprogram.name )
    if (!fn->getReturnType()->isVoidTy()) {
        if (program_this.count(pSubprogram->head->name->content) ||
            program_const_this.count(pSubprogram->head->name->content)) {
            llvm_pascal_s_report_semantic_error_n(
                    pSubprogram->head->name,
                    fmt::format("ident redeclared"));
        }

        llvm::IRBuilder<> dfn_block(&fn->getEntryBlock(), fn->getEntryBlock().begin());
        program_this[pSubprogram->head->name->content] = dfn_block.CreateAlloca(
                fn->getReturnType(), nullptr, pSubprogram->head->name->content);
    }

    // 重新设置builder指向的basic block，虽然没有函数嵌套，但执行这个动作可以保证生成是正确的
    // reset_insert_point(function_entry)
    ir_builder.SetInsertPoint(body);

    // generate body
    if (pSubprogram->body->compound->stmts->stmts.empty() ||
        code_gen_statement(pSubprogram->body->compound)) {
        // if body generated
        if (fn->getReturnType()->isVoidTy()) {
            // out_code( ret void )
            ir_builder.CreateRetVoid();
        } else {
            // out_code( %ret_tmp = load ret_type from %subprogram.name )
            // out_code( ret ret_type: %ret_tmp )
            ir_builder.CreateRet(
                    ir_builder.CreateLoad(program_this[pSubprogram->head->name->content], "ret_tmp"));
        }

        llvm::verifyFunction(*fn);
        fn_pass_manager.run(*fn);

        // pop 'this' from scope stack
        scope_stack = link.last;
        return fn;
    }

    fn->eraseFromParent();
    // pop 'this' from scope stack
    scope_stack = link.last;
    return nullptr;
}
