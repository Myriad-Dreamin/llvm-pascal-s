//
// Created by kamiyoru on 2020/5/8.
//

#ifndef PASCAL_S_AST_H
#define PASCAL_S_AST_H

#include <cstdint>
#include <vector>
#include "token.h"

namespace ast {
    enum class Type : uint16_t {
        Unknown,
        Program,
        Procedure,
        Function,
        StatementBlock,
        ExpCall,
        IfElseStatement,
        ForStatement,
        ExpConstantInteger,
        ExpConstantChar,
        ExpConstantBoolean,
        ExpConstantString,
        ExpConstantReal,
        Ident,
        ParamList,
        VariableList,
        IdentList,
        ConstDecl,
        ConstDecls,
        VarDecl,
        VarDecls,
        FunctionDecl,
        FunctionDecls,
        ExpAssign,
        UnExp,
        BiExp,
        BasicTypeSpec,
        ArrayTypeSpec,
    };

    struct Node;

    void deleteAST(Node *node);

    struct Node {
        Type type;

        explicit Node(Type type) : type(type) {}
    };

    struct Exp : public Node {
        explicit Exp(Type type) : Node(type) {}
    };

    struct Function : public Node {
        Function() : Node(Type::Function) {}
    };

    struct TypeSpec: public Node {
        explicit TypeSpec(Type type) : Node(type) {}
    };

    struct BasicTypeSpec: public TypeSpec {
        const Keyword* keyword;
        explicit BasicTypeSpec(const Keyword* keyword) : TypeSpec(Type::BasicTypeSpec), keyword(keyword) {}
    };

    struct ArrayTypeSpec: public TypeSpec {
        const Keyword* keyword;
        std::vector<std::pair<int64_t, int64_t>> periods;
        explicit ArrayTypeSpec(const Keyword* keyword) : TypeSpec(Type::ArrayTypeSpec), keyword(keyword) {}
    };

    struct ParamList : public Node {
        std::vector<Exp *> params;

        ParamList() : Node(Type::ParamList) {}

        ~ParamList() {
            for (auto exp : params) {
                deleteAST(exp);
            }
        }
    };

    struct VariableList : public Node {
        std::vector<Exp *> params;

        VariableList() : Node(Type::VariableList) {}

        ~VariableList() {
            for (auto exp : params) {
                deleteAST(exp);
            }
        }
    };

    struct IdentList : public Node {
        std::vector<const Identifier *> idents;

        IdentList() : Node(Type::IdentList) {}
    };

    struct ConstDecl : public Node {
        const Identifier *ident;
        const Exp *rhs;

        ConstDecl(const Identifier *ident, Exp *rhs) : Node(Type::ConstDecl), ident(ident), rhs(rhs) {}
    };

    struct ConstDecls : public Node {
        std::vector<ConstDecl*> decls;
        ConstDecls() : Node(Type::ConstDecls) {}

        ~ConstDecls() {
            for (auto exp : decls) {
                deleteAST(exp);
            }
        }
    };

    struct VarDecl : public Node {
        TypeSpec *type_spec;
        IdentList *idents;
        VarDecl(IdentList *idents, TypeSpec *type_spec) : Node(Type::VarDecl),
                                                          type_spec(type_spec), idents(idents) {}
        ~VarDecl() {
            deleteAST(idents);
        }
    };

    struct VarDecls : public Node {
        std::vector<VarDecl*> decls;
        VarDecls() : Node(Type::VarDecls) {}

        ~VarDecls() {
            for (auto exp : decls) {
                deleteAST(exp);
            }
        }
    };

    struct FunctionDecl : public Node {
        FunctionDecl() : Node(Type::FunctionDecl) {}
    };

    struct FunctionDecls : public Node {
        std::vector<FunctionDecl*> decls;
        FunctionDecls() : Node(Type::FunctionDecls) {}

        ~FunctionDecls() {
            for (auto exp : decls) {
                deleteAST(exp);
            }
        }
    };

    struct Program : public Function {
        Node fn_type;
        const Keyword *program;
        const Identifier *name;
        ConstDecls *decls;

        explicit Program(const Keyword *program, const Identifier *name, ConstDecls *decls)
                : Function(), fn_type(Type::Program), program(program), name(name), decls(decls) {}

        ~Program() {
            deleteAST(decls);
        }
    };

    struct Procedure : public Function {
        Node fn_type;

        Procedure() : Function(), fn_type(Type::Procedure) {}
    };

    struct Ident : public Exp {
        const Identifier *ident;

        explicit Ident(const Identifier *ident) : Exp(Type::Ident), ident(ident) {}
    };

    struct ExpAssign : public Exp {
        ExpAssign() : Exp(Type::ExpAssign) {}
    };

    struct UnExp : public Exp {
        UnExp() : Exp(Type::UnExp) {}
    };

    struct BiExp : public Exp {
        BiExp() : Exp(Type::BiExp) {}
    };

    struct ExpCall : public Exp {
        ExpCall() : Exp(Type::ExpCall) {}
    };

    struct Statement : public Exp {
        explicit Statement(Type type) : Exp(type) {}
    };

    struct StatementBlock : public Statement {
        StatementBlock() : Statement(Type::StatementBlock) {}
    };

    struct IfElseStatement : public Statement {
        IfElseStatement() : Statement(Type::IfElseStatement) {}
    };

    struct ForStatement : public Statement {
        ForStatement() : Statement(Type::ForStatement) {}
    };

    struct ExpConstantInteger: public Exp {
        const ConstantInteger *value;

        explicit ExpConstantInteger(const ConstantInteger *value) : Exp(Type::ExpConstantInteger), value(value) {}
    };

    struct ExpConstantChar: public Exp {
        const ConstantChar *value;

        explicit ExpConstantChar(const ConstantChar *value) : Exp(Type::ExpConstantChar), value(value) {}
    };

    struct ExpConstantBoolean: public Exp {
        const ConstantBoolean *value;

        explicit ExpConstantBoolean(const ConstantBoolean *value) : Exp(Type::ExpConstantBoolean), value(value) {}
    };

    struct ExpConstantString: public Exp {
        const ConstantString *value;

        explicit ExpConstantString(const ConstantString *value) : Exp(Type::ExpConstantString), value(value) {}
    };

    struct ExpConstantReal: public Exp {
        const ConstantReal *value;

        explicit ExpConstantReal(const ConstantReal *value) : Exp(Type::ExpConstantReal), value(value) {}
    };
}

#endif //PASCAL_S_AST_H
