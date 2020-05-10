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
#include <stdexcept>
#include <pascal-s/ast.h>
#include <iostream>

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
        std::map<std::string, llvm::AllocaInst *> *ctx;
        std::map<std::string, Value *> *const_ctx;
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

    void insert_var_decls(Function *cur_func, std::map<std::string, llvm::AllocaInst *> &map, ast::VarDecls *pDecls) {

        if (pDecls != nullptr) {
            llvm::IRBuilder<> TmpB(&cur_func->getEntryBlock(),
                                   cur_func->getEntryBlock().begin());

            for (auto decl : pDecls->decls) {
                llvm::Type *value = create_type(decl->type_spec);
                for (auto ident : decl->idents->idents) {
                    map[ident->content] = TmpB.CreateAlloca(value, nullptr, ident->content);
                }
            }
        }
    }

    void insert_const_decls(std::map<std::string, llvm::Value *> &map, ast::ConstDecls *pDecls) {
        if (pDecls != nullptr) {
            for (auto decl : pDecls->decls) {
                map[decl->ident->content] = code_gen(decl->rhs);
            }
        }
    }

    llvm::Type *create_type(const ast::TypeSpec *spec) {
        if (spec->type == ast::Type::BasicTypeSpec) {
            return create_basic_type(reinterpret_cast<const ast::BasicTypeSpec *>(spec));
        } else if (spec->type == ast::Type::ArrayTypeSpec) {
            assert(false);
            return nullptr;
        } else {
            assert(false);
            return nullptr;
        }
    }

    llvm::Type *create_basic_type(const ast::BasicTypeSpec *spec) {
        switch (spec->keyword->key_type) {
            case KeywordType::Integer:

                return llvm::Type::getInt64Ty(ctx);
            case KeywordType::Real:
                return llvm::Type::getDoubleTy(ctx);
            case KeywordType::Char:
                // fallthrough
            case KeywordType::Boolean:
                return llvm::Type::getInt8Ty(ctx);
//            case KeywordType::String:
            default:
                assert(false);
                return nullptr;
        }
    }

    Function *code_gen_program(const ast::Program *pProgram) {
        Function *program = modules.getFunction(pProgram->name->content);

        if (!program) {
            auto *prototype = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx), false);

            program = Function::Create(prototype, Function::ExternalLinkage,
                                       pProgram->name->content, modules);
        }

        llvm::BasicBlock *body = llvm::BasicBlock::Create(ctx, "entry", program);
        ir_builder.SetInsertPoint(body);

        std::map<std::string, llvm::AllocaInst *> program_this;
        insert_var_decls(program, program_this, pProgram->var_decls);
        std::map<std::string, Value *> program_const_this;
        insert_const_decls(program_const_this, pProgram->const_decls);

        llvm::IRBuilder<> TmpB(&program->getEntryBlock(),
                               program->getEntryBlock().begin());
//llvm::Constant::getIntegerValue(
//                llvm::Type::getInt32Ty(ctx), llvm::APInt(32, 0))
        program_this[pProgram->name->content] = TmpB.CreateAlloca(llvm::Type::getInt32Ty(
                ctx), nullptr, pProgram->name->content);

        auto link = LinkedContext{nullptr, &program_this, &program_const_this};
        linked_ctx = &link;
        if (code_gen_statement(pProgram->body)) {
            ir_builder.CreateRet(
                    ir_builder.CreateLoad(program_this[pProgram->name->content], "ret_tmp"));

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

    Value *code_gen_procedure(const ast::Procedure *pProcedure) {
        return nullptr;
    }

    Value *code_gen_function(const ast::Function *pFunction) {
        switch (pFunction->fn_def->key_type) {
            case KeywordType::Program:
                return code_gen_program(
                        reinterpret_cast<const ast::Program *>(pFunction));
            case KeywordType::Procedure:
                return code_gen_procedure(
                        reinterpret_cast<const ast::Procedure *>(pFunction));
            default:
                assert(false);
                return nullptr;
        }
    }

    Value *code_gen_statement_block(const ast::StatementBlock *pBlock) {
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

    Value *code_gen_exp_call(const ast::ExpCall *pCall) {
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
                argument_value = get_lvalue_pointer(pCall->params->params[i]);
            } else {
                argument_value = code_gen(pCall->params->params[i]);
            }
            if (argument_value == nullptr) {
                assert(false);
                return nullptr;
            }
            if (argument_proto->getType()->getTypeID() != argument_value->getType()->getTypeID()) {
                assert(false);
                return nullptr;
            }
            args.push_back(argument_value);
        }

        return ir_builder.CreateCall(calleeFunc, args, pCall->fn->content);
    }

    Value *code_gen_exec_statement(const ast::ExecStatement *pStatement) {
        return code_gen(pStatement->exp);
    }

    Value *code_gen_if_else_statement(const ast::IfElseStatement *pStatement) {
        return nullptr;
    }

    Value *code_gen_for_statement(const ast::ForStatement *pStatement) {
        return nullptr;
    }

    Value *code_gen_binary_exp(const ast::BiExp *pExp) {
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
    }

    Value *code_gen_unary_exp(const ast::UnExp *pExp) {
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
    }

    Value *code_gen_exp_assign(const ast::ExpAssign *pAssign) {
        auto rhs = code_gen(pAssign->rhs);
        if (rhs == nullptr) {
            assert(false);
            return nullptr;
        }
        return assign_lvalue(pAssign->lhs, rhs);
    }

    Value *assign_named_value(const char *content, Value *rhs) {
        for (auto resolving_ctx = linked_ctx;
             resolving_ctx; resolving_ctx = resolving_ctx->last) {
            auto &value_ctx = resolving_ctx->ctx;
            if (value_ctx->count(content)) {
                auto ptr = (*value_ctx)[content];
                auto elemType = ptr->getType()->getElementType();

                if (rhs->getType()->getTypeID() != elemType->getTypeID()) {
                    assert(false);
                    return nullptr;
                }
                if (elemType->getTypeID() == llvm::Type::IntegerTyID) {
                    auto rhs_bit_width = rhs->getType()->getIntegerBitWidth();
                    auto elem_type_bit_width = elemType->getIntegerBitWidth();
                    if (rhs_bit_width > elem_type_bit_width) {
                        rhs = ir_builder.CreateTrunc(rhs, elemType, "");
                    } else {
                        // todo zero ext
                        rhs = ir_builder.CreateSExt(rhs, elemType, "");
                    }

                }

                ir_builder.CreateStore(rhs, ptr);

                // rhs or load(lhs) ?
                return rhs;
            }
            auto &const_ctx = resolving_ctx->const_ctx;
            if (const_ctx->count(content)) {
                assert(false);
                throw std::runtime_error("const value assigned");
            }
        }
        return nullptr;
    }

    Value *assign_lvalue(const ast::Exp *lvalue, Value *rhs) {
        switch (lvalue->type) {
            case ast::Type::Ident:
                return assign_named_value(
                        reinterpret_cast<const ast::Ident *>(lvalue)->ident->content, rhs);
            default:
                assert(false);
                return nullptr;
        }
    }

    Value *get_lvalue_pointer(const ast::Exp *lvalue) {
        switch (lvalue->type) {
            case ast::Type::Ident:
                return get_named_value_pointer(
                        reinterpret_cast<const ast::Ident *>(lvalue)->ident->content);
            default:
                assert(false);
                return nullptr;
        }
    }

    Value *get_named_value_pointer(const char *name) {
        for (auto resolving_ctx = linked_ctx;
             resolving_ctx; resolving_ctx = resolving_ctx->last) {
            auto &value_ctx = resolving_ctx->ctx;
            if (value_ctx->count(name)) {
                auto ptr = ir_builder.CreateLoad((*value_ctx).at(name));
                return ptr->getPointerOperand();
            }
            auto &const_ctx = resolving_ctx->const_ctx;
            if (const_ctx->count(name)) {
                assert(false);
                throw std::runtime_error("const value is not addressable");
            }
        }
        return nullptr;
    }

    Value *code_gen_exp_constant_integer(const ast::ExpConstantInteger *pInteger) {
        return llvm::Constant::getIntegerValue(
                llvm::Type::getInt64Ty(ctx), llvm::APInt(64, pInteger->value->attr));
    }

    Value *code_gen_exp_constant_char(const ast::ExpConstantChar *pChar) {
        return llvm::Constant::getIntegerValue(
                llvm::Type::getInt8Ty(ctx), llvm::APInt(8, pChar->value->value));
    }

    Value *code_gen_exp_constant_real(const ast::ExpConstantReal *pReal) {
        return llvm::ConstantFP::get(
                llvm::Type::getDoubleTy(ctx), llvm::APFloat(pReal->value->attr));
    }

    Value *code_gen_exp_constant_boolean(const ast::ExpConstantBoolean *pBoolean) {
        return llvm::Constant::getIntegerValue(
                llvm::Type::getInt8Ty(ctx), llvm::APInt(8, pBoolean->value->attr));
    }

    static Value *code_gen_exp_constant_string(const ast::ExpConstantString *) {
        assert(false);
        return nullptr;
    }

    Value *code_gen_ident(const ast::Ident *pIdent) {
        for (auto resolving_ctx = linked_ctx;
             resolving_ctx; resolving_ctx = resolving_ctx->last) {
            auto &value_ctx = resolving_ctx->ctx;
            if (value_ctx->count(pIdent->ident->content)) {
                return ir_builder.CreateLoad(value_ctx->at(pIdent->ident->content), "load_tmp");
            }
            auto &const_ctx = resolving_ctx->const_ctx;
            if (const_ctx->count(pIdent->ident->content)) {
                return const_ctx->at(pIdent->ident->content);
            }
        }
        //todo: function call
        return nullptr;
    }

//    Value *code_gen_variable_list(const ast::VariableList *pList) {
//        return nullptr;
//    }
//
//    Value *code_gen_function_decls(const ast::FunctionDecls *pDecls) {
//        return nullptr;
//    }
//
//    Value *code_gen_param_list(const ast::ParamList *pList) {
//        return nullptr;
//    }
//
//    Value *code_gen_ident_list(const ast::IdentList *pList) {
//        return nullptr;
//    }
//
//    Value *code_gen_const_decl(const ast::ConstDecl *pDecl) {
//        return nullptr;
//    }
//
//    Value *code_gen_const_decls(const ast::ConstDecls *pDecls) {
//        return nullptr;
//    }
//
//    Value *code_gen_var_decl(const ast::VarDecl *pDecl) {
//        return nullptr;
//    }
//
//    Value *code_gen_var_decls(const ast::VarDecls *pDecls) {
//        return nullptr;
//    }

    Value *code_gen_statement(const ast::Statement *stmt) {
        switch (stmt->type) {
            case ast::Type::StatementBlock:
                return code_gen_statement_block(
                        reinterpret_cast<const ast::StatementBlock *>(stmt));
            case ast::Type::ExecStatement:
                return code_gen_exec_statement(
                        reinterpret_cast<const ast::ExecStatement *>(stmt));
            case ast::Type::IfElseStatement:
                return code_gen_if_else_statement(
                        reinterpret_cast<const ast::IfElseStatement *>(stmt));
            case ast::Type::ForStatement:
                return code_gen_for_statement(
                        reinterpret_cast<const ast::ForStatement *>(stmt));
            default:
                assert(false);
                return nullptr;
        }
    }

    Value *code_gen(const ast::Node *node) {
        switch (node->type) {
            default:
                assert(false);
                return nullptr;
            case ast::Type::Unknown:
                assert(false);
                return nullptr;
            case ast::Type::Program:
                return code_gen_program(
                        reinterpret_cast<const ast::Program *>(node));
            case ast::Type::Procedure:
                return code_gen_procedure(
                        reinterpret_cast<const ast::Procedure *>(node));
            case ast::Type::Function:
                return code_gen_function(
                        reinterpret_cast<const ast::Function *>(node));
            case ast::Type::StatementBlock:
                return code_gen_statement_block(
                        reinterpret_cast<const ast::StatementBlock *>(node));
            case ast::Type::ExpCall:
                return code_gen_exp_call(
                        reinterpret_cast<const ast::ExpCall *>(node));
            case ast::Type::ExecStatement:
                return code_gen_exec_statement(
                        reinterpret_cast<const ast::ExecStatement *>(node));
            case ast::Type::IfElseStatement:
                return code_gen_if_else_statement(
                        reinterpret_cast<const ast::IfElseStatement *>(node));
            case ast::Type::ForStatement:
                return code_gen_for_statement(
                        reinterpret_cast<const ast::ForStatement *>(node));
            case ast::Type::ExpConstantInteger:
                return code_gen_exp_constant_integer(
                        reinterpret_cast<const ast::ExpConstantInteger *>(node));
            case ast::Type::ExpConstantChar:
                return code_gen_exp_constant_char(
                        reinterpret_cast<const ast::ExpConstantChar *>(node));
            case ast::Type::ExpConstantBoolean:
                return code_gen_exp_constant_boolean(
                        reinterpret_cast<const ast::ExpConstantBoolean *>(node));
            case ast::Type::ExpConstantString:
                return code_gen_exp_constant_string(
                        reinterpret_cast<const ast::ExpConstantString *>(node));
            case ast::Type::ExpConstantReal:
                return code_gen_exp_constant_real(
                        reinterpret_cast<const ast::ExpConstantReal *>(node));
            case ast::Type::Ident:
                return code_gen_ident(
                        reinterpret_cast<const ast::Ident *>(node));
//            case ast::Type::ParamList:
//                return code_gen_param_list(
//                        reinterpret_cast<const ast::ParamList *>(node));
//            case ast::Type::VariableList:
//                return code_gen_variable_list(
//                        reinterpret_cast<const ast::VariableList *>(node));
//            case ast::Type::IdentList:
//                return code_gen_ident_list(
//                        reinterpret_cast<const ast::IdentList *>(node));
//            case ast::Type::ConstDecl:
//                return code_gen_const_decl(
//                        reinterpret_cast<const ast::ConstDecl *>(node));
//            case ast::Type::ConstDecls:
//                return code_gen_const_decls(
//                        reinterpret_cast<const ast::ConstDecls *>(node));
//            case ast::Type::VarDecl:
//                return code_gen_var_decl(
//                        reinterpret_cast<const ast::VarDecl *>(node));
//            case ast::Type::VarDecls:
//                return code_gen_var_decls(
//                        reinterpret_cast<const ast::VarDecls *>(node));
//            case ast::Type::FunctionDecls:
//                return code_gen_function_decls(
//                        reinterpret_cast<const ast::FunctionDecls *>(node));
            case ast::Type::ExpAssign:
                return code_gen_exp_assign(
                        reinterpret_cast<const ast::ExpAssign *>(node));
            case ast::Type::UnExp:
                return code_gen_unary_exp(
                        reinterpret_cast<const ast::UnExp *>(node));
            case ast::Type::BiExp:
                return code_gen_binary_exp(
                        reinterpret_cast<const ast::BiExp *>(node));
//            case ast::Type::BasicTypeSpec:
//                return code_gen_BasicTypeSpec(
//                        reinterpret_cast<const ast::BasicTypeSpec*>(node));
//            case ast::Type::ArrayTypeSpec:
//                return code_gen_ArrayTypeSpec(
//                        reinterpret_cast<const ast::ArrayTypeSpec*>(node));
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
