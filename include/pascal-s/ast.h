//
// Created by kamiyoru on 2020/5/8.
//

#ifndef KAL_AST_H
#define KAL_AST_H

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
        ExecStatement,
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
        FunctionDecls,
        ExpAssign,
        UnExp,
        BiExp,
        BasicTypeSpec,
        ArrayTypeSpec,
    };

    struct Node;

    void deleteAST(Node *node);

    void printAST(Node *node, int dep = 0);

    struct Node {
        Type type;

        explicit Node(Type type) : type(type) {}
    };

    struct Exp : public Node {
        explicit Exp(Type type) : Node(type) {}
    };

    struct Statement : public Exp {
        explicit Statement(Type type) : Exp(type) {}
    };

    struct Function : public Node {
        const Keyword *fn_def;

        explicit Function(const Keyword *fn_def) : fn_def(fn_def), Node(Type::Function) {}
    };

    struct TypeSpec : public Node {
        explicit TypeSpec(Type type) : Node(type) {}
    };

    struct BasicTypeSpec : public TypeSpec {
        const Keyword *keyword;

        explicit BasicTypeSpec(const Keyword *keyword) : TypeSpec(Type::BasicTypeSpec), keyword(keyword) {}
    };

    struct ArrayTypeSpec : public TypeSpec {
        const Keyword *keyword;
        std::vector<std::pair<const ConstantInteger *, const ConstantInteger *>> periods;

        explicit ArrayTypeSpec(
                std::vector<std::pair<const ConstantInteger *, const ConstantInteger *>> &periods,
                const Keyword *keyword) : TypeSpec(Type::ArrayTypeSpec), periods(std::move(periods)),
                                          keyword(keyword) {}
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
        std::vector<ConstDecl *> decls;

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
        std::vector<VarDecl *> decls;

        VarDecls() : Node(Type::VarDecls) {}

        ~VarDecls() {
            for (auto exp : decls) {
                deleteAST(exp);
            }
        }
    };

    struct Procedure : public Function {
        Type fn_type;
        const Identifier *name;
        ParamList *params;
        Statement *body;


        explicit Procedure(const Keyword *fn_def, const Identifier *name)
                : Function(fn_def), fn_type(Type::Procedure), name(name), params(nullptr), body(nullptr) {}

        ~Procedure() {
            deleteAST(params);
            deleteAST(body);
        }
    };

    struct FunctionDecls : public Node {
        std::vector<Procedure *> decls;

        FunctionDecls() : Node(Type::FunctionDecls) {}

        ~FunctionDecls() {
            for (auto exp : decls) {
                deleteAST(exp);
            }
        }
    };

    struct Program : public Function {
        Type fn_type;
        const Identifier *name;
        IdentList *idents;
        ConstDecls *const_decls;
        VarDecls *var_decls;
        FunctionDecls *fn_decls;
        Statement *body;

        explicit Program(const Keyword *program, const Identifier *name, IdentList *idents,
                         ConstDecls *decls, VarDecls *var_decls, FunctionDecls *fn_decls,
                         Statement *body)
                : Function(program), fn_type(Type::Program), name(name), idents(idents),
                  const_decls(decls), var_decls(var_decls), fn_decls(fn_decls), body(body) {}

        ~Program() {
            deleteAST(idents);
            deleteAST(const_decls);
            deleteAST(var_decls);
            deleteAST(fn_decls);
            deleteAST(body);
        }
    };

    struct Ident : public Exp {
        const Identifier *ident;

        explicit Ident(const Identifier *ident) : Exp(Type::Ident), ident(ident) {}
    };

    struct ExpAssign : public Exp {
        ast::Exp *lhs, *rhs;

        ExpAssign(ast::Exp *lhs, ast::Exp *rhs) : Exp(Type::ExpAssign), lhs(lhs), rhs(rhs) {}

        ~ExpAssign() {
            deleteAST(lhs);
            deleteAST(rhs);
        }
    };

    struct UnExp : public Exp {
        Exp *lhs;
        const Marker *marker;

        explicit UnExp(const Marker *marker, Exp *lhs) : Exp(Type::UnExp), lhs(lhs), marker(marker) {}

        ~UnExp() {
            deleteAST(lhs);
        };
    };

    struct BiExp : public Exp {
        Exp *lhs, *rhs;
        const Marker *marker;

        explicit BiExp(Exp *lhs, const Marker *marker, Exp *rhs) : Exp(Type::BiExp), lhs(lhs), rhs(rhs),
                                                                   marker(marker) {}

        ~BiExp() {
            deleteAST(lhs);
            deleteAST(rhs);
        };
    };

    struct ExpCall : public Exp {
        const Identifier *fn;
        VariableList *params;

        explicit ExpCall(const Identifier *fn, VariableList *params) : Exp(Type::ExpCall), fn(fn), params(params) {}

        ~ExpCall() {
            deleteAST(params);
        };
    };

    struct StatementBlock : public Statement {
        const Keyword *key_begin, *key_end;
        std::vector<Statement *> stmts;

        StatementBlock(const Keyword *key_begin, const Keyword *key_end) : Statement(Type::StatementBlock),
                                                                   key_begin(key_begin), key_end(key_end) {}

        ~StatementBlock() {
            for (auto stmt: stmts) {
                deleteAST(stmt);
            }
        }
    };

    struct ExecStatement : public Statement {
        Exp *exp;
        explicit ExecStatement(Exp *exp) : Statement(Type::ExecStatement), exp(exp) {}
        ~ExecStatement() {
            deleteAST(exp);
        }
    };

    struct IfElseStatement : public Statement {
        IfElseStatement() : Statement(Type::IfElseStatement) {}
    };

    struct ForStatement : public Statement {
        ForStatement() : Statement(Type::ForStatement) {}
    };

    struct ExpConstantInteger : public Exp {
        const ConstantInteger *value;

        explicit ExpConstantInteger(const ConstantInteger *value) : Exp(Type::ExpConstantInteger), value(value) {}
    };

    struct ExpConstantChar : public Exp {
        const ConstantChar *value;

        explicit ExpConstantChar(const ConstantChar *value) : Exp(Type::ExpConstantChar), value(value) {}
    };

    struct ExpConstantBoolean : public Exp {
        const ConstantBoolean *value;

        explicit ExpConstantBoolean(const ConstantBoolean *value) : Exp(Type::ExpConstantBoolean), value(value) {}
    };

    struct ExpConstantString : public Exp {
        const ConstantString *value;

        explicit ExpConstantString(const ConstantString *value) : Exp(Type::ExpConstantString), value(value) {}
    };

    struct ExpConstantReal : public Exp {
        const ConstantReal *value;

        explicit ExpConstantReal(const ConstantReal *value) : Exp(Type::ExpConstantReal), value(value) {}
    };
}

#endif //KAL_AST_H
