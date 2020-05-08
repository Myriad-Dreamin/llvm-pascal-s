//
// Created by kamiyoru on 2020/5/8.
//

#include <pascal-s/ast.h>
#include <pascal-s/exception.h>

void ast::deleteAST(Node *node) {
    if (node == nullptr) {
        return;
    }
    switch (node->type) {
        default:
        case Type::Unknown:
            throw RuntimeReinterpretASTException(node);
        case Type::Program:
            delete reinterpret_cast<ast::Program*>(node);
            break;
        case Type::Procedure:
            delete reinterpret_cast<ast::Procedure*>(node);
            break;
        case Type::Function:
            delete reinterpret_cast<ast::Function*>(node);
            break;
        case Type::StatementBlock:
            delete reinterpret_cast<ast::StatementBlock*>(node);
            break;
        case Type::ExpCall:
            delete reinterpret_cast<ast::ExpCall*>(node);
            break;
        case Type::ExecStatement:
            delete reinterpret_cast<ast::ExecStatement*>(node);
            break;
        case Type::IfElseStatement:
            delete reinterpret_cast<ast::IfElseStatement*>(node);
            break;
        case Type::ForStatement:
            delete reinterpret_cast<ast::ForStatement*>(node);
            break;
        case Type::Ident:
            delete reinterpret_cast<ast::Ident*>(node);
            break;
        case Type::ParamList:
            delete reinterpret_cast<ast::ParamList*>(node);
            break;
        case Type::VariableList:
            delete reinterpret_cast<ast::VariableList*>(node);
            break;
        case Type::IdentList:
            delete reinterpret_cast<ast::IdentList*>(node);
            break;
        case Type::ConstDecl:
            delete reinterpret_cast<ast::ConstDecl*>(node);
            break;
        case Type::ConstDecls:
            delete reinterpret_cast<ast::ConstDecls*>(node);
            break;
        case Type::VarDecl:
            delete reinterpret_cast<ast::VarDecl*>(node);
            break;
        case Type::VarDecls:
            delete reinterpret_cast<ast::VarDecls*>(node);
            break;
        case Type::FunctionDecls:
            delete reinterpret_cast<ast::FunctionDecls*>(node);
            break;
        case Type::ExpAssign:
            delete reinterpret_cast<ast::ExpAssign*>(node);
            break;
        case Type::UnExp:
            delete reinterpret_cast<ast::UnExp*>(node);
            break;
        case Type::BiExp:
            delete reinterpret_cast<ast::BiExp*>(node);
            break;
        case Type::ExpConstantBoolean:
            delete reinterpret_cast<ast::ExpConstantBoolean*>(node);
            break;
        case Type::ExpConstantChar:
            delete reinterpret_cast<ast::ExpConstantChar*>(node);
            break;
        case Type::ExpConstantInteger:
            delete reinterpret_cast<ast::ExpConstantInteger*>(node);
            break;
        case Type::ExpConstantString:
            delete reinterpret_cast<ast::ExpConstantString*>(node);
            break;
        case Type::ExpConstantReal:
            delete reinterpret_cast<ast::ExpConstantReal*>(node);
            break;
        case Type::BasicTypeSpec:
            delete reinterpret_cast<ast::BasicTypeSpec*>(node);
            break;
        case Type::ArrayTypeSpec:
            delete reinterpret_cast<ast::ArrayTypeSpec*>(node);
            break;


    }
}
