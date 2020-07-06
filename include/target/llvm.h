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
#include <pascal-s/llvm-ast.h>
#include <iostream>
#include "llvm/IR/LegacyPassManager.h"
#include <pascal-s/exception.h>

extern const char *pascal_main_function_name;

namespace pascal_s {
    struct ArrayInfo {
        const ast::ArrayTypeSpec *spec;
    };
}

llvm::Value *create_int64_literal(llvm::LLVMContext &ctx, int64_t literal);

std::string format_type(llvm::Type *t);

// 用于生成目标中间代码LLVM-IR
struct LLVMBuilder {
    using LLVMContext = llvm::LLVMContext;
    using Module = llvm::Module;
    using Function = llvm::Function;
    using Value = llvm::Value;
    using Type = llvm::Type;

    //编译用数据结构
    // ctx: 编译变量上下文
    LLVMContext ctx;
    // ir builder: 指令生成控制层
    llvm::IRBuilder<> ir_builder;
    // function Pass manager: 函数优化管理器
    llvm::legacy::FunctionPassManager fn_pass_manager;
    // 每个文件都是单独的module，module间共享接口，独立实现
    Module modules;
    // 语义错误
    std::vector<PascalSSemanticError *> errors;
    // 语义警告
    std::vector<PascalSSemanticError *> warnings;

    //访问链结构体
    struct LinkedContext {
        LinkedContext *last = nullptr;
        std::map<std::string, pascal_s::ArrayInfo *> *array_infos;
        std::map<std::string, llvm::Value *> *ctx;
        std::map<std::string, Value *> *const_ctx;

        ~LinkedContext() {
            deconstruct();
        }

        void deconstruct() const;
    };

    //访问链
    LinkedContext *scope_stack;

    LLVMBuilder();

    ~LLVMBuilder();

    void report_semantic_error(const char *fn, const pascal_s::Pos *, std::string &&msg);

    void report_semantic_warning(const char *fn, const pascal_s::Pos *, std::string &&msg);

#define llvm_pascal_s_report_semantic_error(pos, msg) report_semantic_error(__FUNCTION__, pos, msg)
#define llvm_pascal_s_report_semantic_warning(pos, msg) report_semantic_warning(__FUNCTION__, pos, msg)
#define llvm_pascal_s_report_semantic_error_n(node, msg) llvm_pascal_s_report_semantic_error(node->visit_pos(), msg)
#define llvm_pascal_s_report_semantic_warning_n(node, msg) llvm_pascal_s_report_semantic_warning(node->visit_pos(), msg)

    // llvm相关初始化
    // do_init: 初始化llvm模块
    // prepend_lib_standard_pascal_s: 声明内置函数
    bool doInitialization();

    void prepend_lib_standard_pascal_s();

    // llvm辅助函数

    Function *rename_program_to_pascal_s_native(Function *f);

    Value *assign_lvalue(const pascal_s::Pos *lvalue_pos, Value *ptr, Value *rhs);

    bool
    check_extend_type(const pascal_s::Pos *pos, llvm::Value *&source, llvm::Value *&target, bool target_changeable);

    bool check_extend_type(const pascal_s::Pos *pos, llvm::Value *&source, llvm::Type *target);

    // 更新访问链信息
    // var_decl: 可变变量
    // const_decl: 运行时常量（可以在编译时确认）
    // param_decl: 函数输入局部变量
    void insert_var_decls(Function *cur_func, std::map<std::string, pascal_s::ArrayInfo *> &array_infos,
                          std::map<std::string, llvm::Value *> &map, const ast::VarDecls *pDecls,
                          bool on_stack = false);

    void insert_const_decls(std::map<std::string, llvm::Value *> &map, const ast::ConstDecls *pDecls);

    void insert_param_decls(
            const pascal_s::Pos *pos, Function *fn, std::map<std::string, llvm::Value *> &map,
            std::map<std::string, llvm::Value *> &const_map);

    // 计算数组信息

    int64_t calc_periods_size(const ast::ArrayTypeSpec *spec);

    void code_gen_offset(std::vector<llvm::Value *> &offset, const pascal_s::ArrayInfo *ai,
                         const ast::ExpressionList *exp_list);

    // 生成目标代码辅助函数
    // abstract: 抽象基类
    // type: 类型信息
    // function: 函数
    // statement: 语句
    // expression: 表达式
    // factor: 表达式因子（基本类型）

    // abstract node entry

    Value *code_gen(const ast::Node *node);

    // ast::Function function: -> llvm::Function
    Function *code_gen_function(const ast::Function *pFunction);

    Value *code_gen_statement(const ast::Statement *stmt);

    // gen type

    Type *create_type(const ast::TypeSpec *spec);

    Type *create_basic_type(const ast::BasicTypeSpec *spec);

    Type *create_basic_type_from_keyword(const Keyword *spec);

    // gen function

    // ast::Program program: -> llvm::Function
    Function *code_gen_program(const ast::Program *program);

    // ast::Subprogram subprogram: -> llvm::Function
    Function *code_gen_subprogram(const ast::Subprogram *subprogram);

    // gen statement

    // ast::StatementBlock block: -> llvm::Value
    Value *code_gen_statement_block(const ast::StatementList *block);

    // ast::ExecStatement exec_stmt: -> llvm::Value
    Value *code_gen_exec_statement(const ast::ExecStatement *exec_stmt);

    // ast::IfElseStatement if_else_stmt: -> llvm::Value
    Value *code_gen_if_else_statement(const ast::IfElseStatement *if_else_stmt);

    // ast::ForStatement for_stmt: -> llvm::Value
    Value *code_gen_for_statement(const ast::ForStatement *for_stmt);

    // ast::ReadStatement read_stmt: -> llvm::Value
    Value *code_gen_read_statement(const ast::Read *read_stmt);

    // ast::WriteStatement write_stmt: -> llvm::Value
    Value *code_gen_write_statement(const ast::Write *write_stmt);

    // gen complex expression

    // ast::ExpCall call_stmt: -> llvm::Value
    Value *code_gen_exp_call(const ast::ExpCall *call_stmt);

    // ast::ExpAssign assign_exp: -> llvm::Value
    Value *code_gen_exp_assign(const ast::ExpAssign *bi_exp);

    // ast::ExpCall bi_exp: -> llvm::Value
    Value *code_gen_binary_exp(const ast::BiExp *bi_exp);

    // ast::UnExp un_exp: -> llvm::Value
    Value *code_gen_unary_exp(const ast::UnExp *un_exp);

    // gen expression factor

    // ast::ConstantXXX const: -> llvm::Value
    Value *code_gen_exp_constant_integer(const ast::ExpConstantInteger *pInteger);

    // ast::ConstantXXX const: -> llvm::Value
    Value *code_gen_exp_constant_char(const ast::ExpConstantChar *pChar);

    // ast::ConstantXXX const: -> llvm::Value
    Value *code_gen_exp_constant_real(const ast::ExpConstantReal *pReal);

    // ast::ConstantXXX const: -> llvm::Value
    Value *code_gen_exp_constant_boolean(const ast::ExpConstantBoolean *pBoolean);

    // ast::ConstantXXX const: -> llvm::Value
    Value *code_gen_exp_constant_string(const ast::ExpConstantString *);

    // ast::ExpIdent ident: -> llvm::Value
    Value *code_gen_ident(const ast::Ident *pIdent);

    // ast::Exp lvalue: -> llvm::Value
    Value *get_lvalue_pointer(const ast::Exp *lvalue);

    // ast::Variable var: -> llvm::Value
    Value *code_gen_variable(const ast::Variable *var);
};


#endif //PASCAL_S_LLVM_H
