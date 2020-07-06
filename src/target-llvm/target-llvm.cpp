//
// Created by kamiyoru on 2020/5/10.
//

#include <target/llvm.h>
#include <fmt/core.h>

LLVMBuilder::LLVMBuilder() :
        ir_builder(ctx), modules("llvm-pascal-s", ctx), fn_pass_manager(&modules), scope_stack(nullptr) {
    prepend_lib_standard_pascal_s();
}

LLVMBuilder::~LLVMBuilder() {
    for (auto e: errors) {
        delete e;
    }
}

bool LLVMBuilder::doInitialization() {
    return fn_pass_manager.doInitialization();
}

LLVMBuilder::Function *LLVMBuilder::code_gen_function(const ast::Function *pFunction) {
    switch (pFunction->type) {
        // return gen_program() if function.type == program
        case ast::Type::Program:
            return code_gen_program(
                    reinterpret_cast<const ast::Program *>(pFunction));
            // else return gen_subprogram() if function.type == subprogram (with ret type or without)
        case ast::Type::Subprogram:
            return code_gen_subprogram(
                    reinterpret_cast<const ast::Subprogram *>(pFunction));
        default:
            llvm_pascal_s_report_semantic_error_n(pFunction, "gen method not found");
            return nullptr;
    }
}

LLVMBuilder::Value *LLVMBuilder::code_gen_statement(const ast::Statement *stmt) {
    switch (stmt->type) {
        case ast::Type::StatementList:
            return code_gen_statement_block(
                    reinterpret_cast<const ast::StatementList *>(stmt));
        case ast::Type::CompoundStatement:
            return code_gen_statement_block(
                    reinterpret_cast<const ast::CompoundStatement *>(stmt)->state);
        case ast::Type::ExecStatement:
            return code_gen_exec_statement(
                    reinterpret_cast<const ast::ExecStatement *>(stmt));
        case ast::Type::IfElseStatement:
            return code_gen_if_else_statement(
                    reinterpret_cast<const ast::IfElseStatement *>(stmt));
        case ast::Type::ForStatement:
            return code_gen_for_statement(
                    reinterpret_cast<const ast::ForStatement *>(stmt));
        case ast::Type::Read:
            return code_gen_read_statement(
                    reinterpret_cast<const ast::Read *>(stmt));
        case ast::Type::Write:
            return code_gen_write_statement(
                    reinterpret_cast<const ast::Write *>(stmt));
        default:
            llvm_pascal_s_report_semantic_error_n(stmt, "gen method not found");
            return nullptr;
    }
}

LLVMBuilder::Value *LLVMBuilder::code_gen(const ast::Node *node) {
    switch (node->type) {
        default:
            llvm_pascal_s_report_semantic_error_n(node, "gen method not found");
            return nullptr;
        case ast::Type::Unknown:
            llvm_pascal_s_report_semantic_error_n(node, "Type::Unknown Node has no gen method");
            return nullptr;
        case ast::Type::Program:
            return code_gen_program(
                    reinterpret_cast<const ast::Program *>(node));
        case ast::Type::Subprogram:
            return code_gen_subprogram(
                    reinterpret_cast<const ast::Subprogram *>(node));
        case ast::Type::Function:
            return code_gen_function(
                    reinterpret_cast<const ast::Function *>(node));
        case ast::Type::StatementList:
            return code_gen_statement_block(
                    reinterpret_cast<const ast::StatementList *>(node));
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
        case ast::Type::CompoundStatement:
            return code_gen_statement_block(
                    reinterpret_cast<const ast::CompoundStatement *>(node)->state);
        case ast::Type::Read:
            return code_gen_read_statement(
                    reinterpret_cast<const ast::Read *>(node));
        case ast::Type::Write:
            return code_gen_write_statement(
                    reinterpret_cast<const ast::Write *>(node));
        case ast::Type::Variable:
            return code_gen_variable(
                    reinterpret_cast<const ast::Variable *>(node));
        case ast::Type::FormalParameter:
            llvm_pascal_s_report_semantic_error_n(node, "gen method not found");
            break;
        case ast::Type::Expression:
            llvm_pascal_s_report_semantic_error_n(node, "gen method not found");
            break;
        case ast::Type::SimpleExpression:
            llvm_pascal_s_report_semantic_error_n(node, "gen method not found");
            break;
        case ast::Type::Procedure:
            llvm_pascal_s_report_semantic_error_n(node, "gen method not found");
            break;
        case ast::Type::Exp:
            llvm_pascal_s_report_semantic_error_n(node, "gen method not found");
            break;
    }
    llvm_pascal_s_report_semantic_error_n(node, "gen method not found");
    return nullptr;
}


void LLVMBuilder::report_semantic_error(const char *fn, const pascal_s::Pos *pos, std::string &&msg) {
    errors.push_back(new PascalSSemanticError(fn, pos,
                                              "semantic error: at function " + (fn + (": " + msg))));
}


void LLVMBuilder::report_semantic_warning(const char *fn, const pascal_s::Pos *pos, std::string &&msg) {
    warnings.push_back(new PascalSSemanticError(fn, pos,
                                                "semantic warning: at function " + (fn + (": " + msg))));
}


