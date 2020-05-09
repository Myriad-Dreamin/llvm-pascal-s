//
// Created by kamiyoru on 2020/5/8.
//

#ifndef PASCAL_S_LLVM_H
#define PASCAL_S_LLVM_H

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <pascal-s/lib/libstdps.h>

#include <pascal-s/ast.h>

extern const char *pascal_main_function_name;

// 用于生成目标中间代码LLVM-IR
struct LLVMBuilder {
    using LLVMContext = llvm::LLVMContext;
    using Module = llvm::Module;
    using Value = llvm::Value;
    using Function = llvm::Function;

    LLVMContext ctx;
    llvm::IRBuilder<> ir_builder;
    Module modules;

    struct LinkedContext {
        LinkedContext *last;
        std::map<std::string, Value *> *ctx;
    };

    LinkedContext *linked_ctx;

    LLVMBuilder() :
            ir_builder(ctx), modules("llvm-pascal-s", ctx), linked_ctx(nullptr) {
        prepend_lib_standard_pascal_s();
    }

    void prepend_lib_standard_pascal_s() {
        auto *prototype = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx), {
                        llvm::Type::getInt8PtrTy(ctx),
                }, false);
        Function::Create(prototype, Function::ExternalLinkage,
                         "read_char", modules);

        prototype = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx), {
                        llvm::Type::getInt64PtrTy(ctx),
                }, false);
        Function::Create(prototype, Function::ExternalLinkage,
                         "read_int64", modules);

        prototype = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx), {
                        llvm::Type::getDoublePtrTy(ctx),
                }, false);
        Function::Create(prototype, Function::ExternalLinkage,
                         "read_real", modules);

        prototype = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx), {
                        llvm::Type::getInt8Ty(ctx),
                }, false);
        Function::Create(prototype, Function::ExternalLinkage,
                         "write_char", modules);

        prototype = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx), {
                        llvm::Type::getInt64Ty(ctx),
                }, false);
        Function::Create(prototype, Function::ExternalLinkage,
                         "write_int64", modules);

        prototype = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(ctx), {
                        llvm::Type::getDoubleTy(ctx),
                }, false);
        Function::Create(prototype, Function::ExternalLinkage,
                         "write_real", modules);
    }

    Function *code_gen_program(ast::Program *pProgram) {
        Function *program = modules.getFunction(pProgram->name->content);

        if (!program) {
            auto *prototype = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx), false);

            program = Function::Create(prototype, Function::ExternalLinkage,
                                       pProgram->name->content, modules);
        }

        llvm::BasicBlock *body = llvm::BasicBlock::Create(ctx, "entry", program);
        ir_builder.SetInsertPoint(body);

        std::map<std::string, Value *> program_this;

        program_this[pProgram->name->content] = llvm::Constant::getIntegerValue(
                llvm::Type::getInt32Ty(ctx), llvm::APInt(32, 0));

        auto link = LinkedContext{nullptr, &program_this};
        linked_ctx = &link;
        if (code_gen_statement(pProgram->body)) {
            ir_builder.CreateRet(program_this[pProgram->name->content]);

            llvm::verifyFunction(*program);
            linked_ctx = nullptr;
            return rename_program_to_pascal_s_native(program);
        }
        program->eraseFromParent();
        linked_ctx = nullptr;
        return nullptr;
    }

    Function *rename_program_to_pascal_s_native(Function *f) const {
        Function *maybe_conflict = modules.getFunction(pascal_main_function_name);
        if (maybe_conflict) {
            assert(false);
            return nullptr;
        }

        f->setName(pascal_main_function_name);
        return f;
    }

    Value *code_gen_procedure(ast::Procedure *pProcedure) {
        return nullptr;
    }

    Value *code_gen_function(ast::Function *pFunction) {
        switch (pFunction->fn_def->key_type) {
            case KeywordType::Program:
                return code_gen_program(
                        reinterpret_cast<ast::Program *>(pFunction));
            case KeywordType::Procedure:
                return code_gen_procedure(
                        reinterpret_cast<ast::Procedure *>(pFunction));
            default:
                assert(false);
                return nullptr;
        }
    }

    Value *code_gen_statement_block(ast::StatementBlock *pBlock) {
//        llvm::BasicBlock *cur_block = ir_builder.GetInsertBlock();
        Value *block_value = nullptr;
        for (auto stmt : pBlock->stmts) {
            block_value = code_gen_statement(stmt);
            if (!block_value) {
                return nullptr;
            }
        }

        return block_value;
    }

    Value *code_gen_exp_call(ast::ExpCall *pCall) {
        Function *calleeFunc = modules.getFunction(pCall->fn->content);
        if (!calleeFunc) {
            assert(false);
            return nullptr;
        }
        if (calleeFunc->arg_size() != pCall->params->params.size()) {
            assert(false);
            return nullptr;
        }
        std::vector<Value *> args;
        for (int i = 0; i < calleeFunc->arg_size(); i++) {
            auto *argument_proto = calleeFunc->getArg(i);
            Value *argument_value = nullptr;
            if (argument_proto->getType()->isPointerTy()) {
                assert(false);
            } else {
                argument_value = code_gen(pCall->params->params[i]);
                if (argument_value == nullptr) {
                    assert(false);
                    return nullptr;
                }
                if (argument_proto->getType()->getTypeID() != argument_value->getType()->getTypeID()) {
                    assert(false);
                    return nullptr;
                }
            }
            args.push_back(argument_value);
        }

        return ir_builder.CreateCall(calleeFunc, args, pCall->fn->content);
    }

    Value *code_gen_exec_statement(ast::ExecStatement *pStatement) {
        return code_gen(pStatement->exp);
    }

    Value *code_gen_if_else_statement(ast::IfElseStatement *pStatement) {
        return nullptr;
    }

    Value *code_gen_for_statement(ast::ForStatement *pStatement) {
        return nullptr;
    }

    Value *code_gen_exp_constant_integer(ast::ExpConstantInteger *pInteger) {
        return llvm::Constant::getIntegerValue(
                llvm::Type::getInt64Ty(ctx), llvm::APInt(64, pInteger->value->attr));
    }

    Value *code_gen_exp_constant_char(ast::ExpConstantChar *pChar) {
        return llvm::Constant::getIntegerValue(
                llvm::Type::getInt8Ty(ctx), llvm::APInt(8, pChar->value->value));
    }

    Value *code_gen_binary_exp(ast::BiExp *pExp) {
        auto lhs = code_gen(pExp->lhs);
        if (lhs == nullptr) {
            assert(false);
            return nullptr;
        }
        auto rhs = code_gen(pExp->rhs);
        if (rhs == nullptr) {
            assert(false);
            return nullptr;
        }

        if (lhs->getType()->getTypeID() != rhs->getType()->getTypeID()) {
            assert(false);
            return nullptr;
        }

        switch (pExp->marker->marker_type) {
            case MarkerType::Add:
                if (lhs->getType()->isIntegerTy()) {
                    return ir_builder.CreateAdd(lhs, rhs, "add_tmp");
                }
                assert(false);
            case MarkerType::Sub:
                if (lhs->getType()->isIntegerTy()) {
                    return ir_builder.CreateSub(lhs, rhs, "sub_tmp");
                }
                assert(false);
            case MarkerType::Mul:
                if (lhs->getType()->isIntegerTy()) {
                    return ir_builder.CreateMul(lhs, rhs, "mul_tmp");
                }
                assert(false);
            case MarkerType::Div:
                if (lhs->getType()->isIntegerTy()) {
                    return ir_builder.CreateUDiv(lhs, rhs, "div_tmp");
                }
                assert(false);
            default:
                assert(false);
                return nullptr;
        }
        return nullptr;
    }

    Value *code_gen_unary_exp(ast::UnExp *pExp) {
        auto lhs = code_gen(pExp->lhs);
        if (lhs == nullptr) {
            assert(false);
            return nullptr;
        }

        switch (pExp->marker->marker_type) {
            case MarkerType::Add:
                return lhs;
            case MarkerType::Sub:
                if (lhs->getType()->isIntegerTy()) {
                    return ir_builder.CreateNeg(lhs, "neg_tmp");
                }
                assert(false);
            default:
                assert(false);
                return nullptr;
        }
        return nullptr;
    }

    Value *code_gen_exp_assign(ast::ExpAssign *pAssign) {
        auto rhs = code_gen(pAssign->rhs);
        if (rhs == nullptr) {
            assert(false);
            return nullptr;
        }
        return assign_lvalue(pAssign->lhs, rhs);
    }

    Value *assign_named_value(const char *content, Value *rhs) const {
        for (auto resolving_ctx = linked_ctx;
             resolving_ctx; resolving_ctx = resolving_ctx->last) {
            auto &value_ctx = resolving_ctx->ctx;
            if (value_ctx->count(content)) {
                (*value_ctx)[content] = rhs;
                return value_ctx->at(content);
            }
        }
        return nullptr;
    }

    Value *assign_lvalue(ast::Exp *lvalue, Value *rhs) const {
        switch (lvalue->type) {
            case ast::Type::Ident:
                return assign_named_value(
                        reinterpret_cast<ast::Ident *>(lvalue)->ident->content, rhs);
            default:
                assert(false);
        }
    }

    Value *code_gen_variable_list(ast::VariableList *pList) {
        return nullptr;
    }

    Value *code_gen_exp_constant_string(ast::ExpConstantString *pString) {
        return nullptr;
    }

    Value *code_gen_function_decls(ast::FunctionDecls *pDecls) {
        return nullptr;
    }

    Value *code_gen_exp_constant_real(ast::ExpConstantReal *pReal) {
        return nullptr;
    }

    Value *code_gen_exp_constant_boolean(ast::ExpConstantBoolean *pBoolean) {
        return nullptr;
    }

    Value *code_gen_ident(ast::Ident *pIdent) {
        return nullptr;
    }

    Value *code_gen_param_list(ast::ParamList *pList) {
        return nullptr;
    }

    Value *code_gen_ident_list(ast::IdentList *pList) {
        return nullptr;
    }

    Value *code_gen_const_decl(ast::ConstDecl *pDecl) {
        return nullptr;
    }

    Value *code_gen_const_decls(ast::ConstDecls *pDecls) {
        return nullptr;
    }

    Value *code_gen_var_decl(ast::VarDecl *pDecl) {
        return nullptr;
    }

    Value *code_gen_var_decls(ast::VarDecls *pDecls) {
        return nullptr;
    }

    Value *code_gen_statement(ast::Statement *stmt) {
        switch (stmt->type) {
            case ast::Type::StatementBlock:
                return code_gen_statement_block(
                        reinterpret_cast<ast::StatementBlock *>(stmt));
            case ast::Type::ExecStatement:
                return code_gen_exec_statement(
                        reinterpret_cast<ast::ExecStatement *>(stmt));
            case ast::Type::IfElseStatement:
                return code_gen_if_else_statement(
                        reinterpret_cast<ast::IfElseStatement *>(stmt));
            case ast::Type::ForStatement:
                return code_gen_for_statement(
                        reinterpret_cast<ast::ForStatement *>(stmt));
            default:
                assert(false);
                return nullptr;
        }
    }

    Value *code_gen(ast::Node *node) {
        switch (node->type) {
            case ast::Type::Unknown:
                break;
            case ast::Type::Program:
                return code_gen_program(
                        reinterpret_cast<ast::Program *>(node));
            case ast::Type::Procedure:
                return code_gen_procedure(
                        reinterpret_cast<ast::Procedure *>(node));
            case ast::Type::Function:
                return code_gen_function(
                        reinterpret_cast<ast::Function *>(node));
            case ast::Type::StatementBlock:
                return code_gen_statement_block(
                        reinterpret_cast<ast::StatementBlock *>(node));
            case ast::Type::ExpCall:
                return code_gen_exp_call(
                        reinterpret_cast<ast::ExpCall *>(node));
            case ast::Type::ExecStatement:
                return code_gen_exec_statement(
                        reinterpret_cast<ast::ExecStatement *>(node));
            case ast::Type::IfElseStatement:
                return code_gen_if_else_statement(
                        reinterpret_cast<ast::IfElseStatement *>(node));
            case ast::Type::ForStatement:
                return code_gen_for_statement(
                        reinterpret_cast<ast::ForStatement *>(node));
            case ast::Type::ExpConstantInteger:
                return code_gen_exp_constant_integer(
                        reinterpret_cast<ast::ExpConstantInteger *>(node));
            case ast::Type::ExpConstantChar:
                return code_gen_exp_constant_char(
                        reinterpret_cast<ast::ExpConstantChar *>(node));
            case ast::Type::ExpConstantBoolean:
                return code_gen_exp_constant_boolean(
                        reinterpret_cast<ast::ExpConstantBoolean *>(node));
            case ast::Type::ExpConstantString:
                return code_gen_exp_constant_string(
                        reinterpret_cast<ast::ExpConstantString *>(node));
            case ast::Type::ExpConstantReal:
                return code_gen_exp_constant_real(
                        reinterpret_cast<ast::ExpConstantReal *>(node));
            case ast::Type::Ident:
                return code_gen_ident(
                        reinterpret_cast<ast::Ident *>(node));
            case ast::Type::ParamList:
                return code_gen_param_list(
                        reinterpret_cast<ast::ParamList *>(node));
            case ast::Type::VariableList:
                return code_gen_variable_list(
                        reinterpret_cast<ast::VariableList *>(node));
            case ast::Type::IdentList:
                return code_gen_ident_list(
                        reinterpret_cast<ast::IdentList *>(node));
            case ast::Type::ConstDecl:
                return code_gen_const_decl(
                        reinterpret_cast<ast::ConstDecl *>(node));
            case ast::Type::ConstDecls:
                return code_gen_const_decls(
                        reinterpret_cast<ast::ConstDecls *>(node));
            case ast::Type::VarDecl:
                return code_gen_var_decl(
                        reinterpret_cast<ast::VarDecl *>(node));
            case ast::Type::VarDecls:
                return code_gen_var_decls(
                        reinterpret_cast<ast::VarDecls *>(node));
            case ast::Type::FunctionDecls:
                return code_gen_function_decls(
                        reinterpret_cast<ast::FunctionDecls *>(node));
            case ast::Type::ExpAssign:
                return code_gen_exp_assign(
                        reinterpret_cast<ast::ExpAssign *>(node));
            case ast::Type::UnExp:
                return code_gen_unary_exp(
                        reinterpret_cast<ast::UnExp *>(node));
            case ast::Type::BiExp:
                return code_gen_binary_exp(
                        reinterpret_cast<ast::BiExp *>(node));
//            case ast::Type::BasicTypeSpec:
//                return code_gen_BasicTypeSpec(
//                        reinterpret_cast<ast::BasicTypeSpec*>(node));
//            case ast::Type::ArrayTypeSpec:
//                return code_gen_ArrayTypeSpec(
//                        reinterpret_cast<ast::ArrayTypeSpec*>(node));
        }

//        switch (node->type) {
//            case NodeType::ExpNumber:
//                return llvm::ConstantFP::get(ctx, llvm::APFloat(
//                        //todo: int
//                        double(reinterpret_cast<ExpNumber*>(node)->num->value)));
//            case NodeType::ExpIdent:
//                return ident_mapping.at(reinterpret_cast<ExpIdent*>(node)->ident->content);
//            case NodeType::BiExp:
//                return code_gen_binary_exp(reinterpret_cast<BiExp*>(node));
//            case NodeType::UnExp:
//                return code_gen_unary_exp(reinterpret_cast<UnExp*>(node));
//            case NodeType::ExpFuncCall:
//                break;
//            default:
//                assert(false);
//                return nullptr;
//        }
//        assert(false);
//        return nullptr;
    }

//    Value *code_gen_unary_exp(UnExp *pExp) {
//        auto lhs = code_gen(pExp->lhs);
//        assert(lhs != nullptr);
//        switch (pExp->marker->marker_type) {
//            case MarkerType::Add:
//                return lhs;
//            case MarkerType::Sub:
//                return ir_builder.CreateFNeg(lhs, "negative_tmp");
//            default :
//                assert(false);
//                return nullptr;
//        }
//    }
};


#endif //PASCAL_S_LLVM_H
