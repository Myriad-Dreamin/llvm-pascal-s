/*
#ifndef PASCAL_S_AST_H

#define PASCAL_S_AST_H



#include "token.h"

#include <vector>
#include <utility>
#include <deque>

enum class Type : uint16_t {

    Unknown,

    Program, // program struct

    ProgramHead, // program head

    ProgramBody, // program body

    IdentList, // id list

    ConstDecls, // const declarations

    ConstDecl, // const declaration

    ExpConst, // const value

    VarDecl, // var declarations

    VarDecls, // var declaration

    TypeSpec, // type

    BasicTypeSpec, // basic type

    ArrayTypeSpec, // array type

    Period, // Period

    SubprogramDecls, // subprogram declarations

    Subprogram, // subprogram

    SubprogramHead, // subprogram head

    SubprogramBody, // subprogram body

    CompoundStatement, // compound_statement

    FormalParameter, // formal parameter

    ParamSpec,//child of parameter list

    ParamList, // parameter list

    StatementList, // statement list

    Statement,

    ElsePart, // else part

    VariableList, // variable list

    Variabele, // variable

    IdVarpart, // id varpart

    ExpressionList, // expression list

    Expression, // expression

    SimpleExpression, // simple expression

    Procedure,

    Function,

    ExecStatement,

    //StatementBlock,

    Read,

    Write,

    ExpCall,

    IfElseStatement,

    ForStatement,

    ExpConstantInteger,

    ExpConstantChar,

    ExpConstantBoolean,

    ExpConstantString,

    ExpConstantReal,

    Ident,

    FunctionDecl,

    FunctionDecls,

    ExpAssign,

    Exp,

    UnExp,

    BiExp,

    ExpMarker,

    ExpKeyword,

    ExpVoid,
};

struct Node;

void deleteAST(Node *node);

void printAST(const Node *node, int dep = 0);


struct Node {

    Type type;

    pascal_s::line_t line = 0;
    // 8 ~ 16字节
    pascal_s::column_t column = 0;
    pascal_s::length_t length = 0;
    // 16 ~ 24字节
    pascal_s::offset_t offset = 0;

    std::deque<Node *> children;

    explicit Node(Type type) : type(type) {}

    char *GetTokenSymbol();

};


struct Exp : public Node {

    explicit Exp(Type type) : Node(type) {}

};


struct Function : public Node {


    explicit Function(Type type) : Node(type) {}

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

    std::vector<std::pair<int64_t, int64_t>> periods;

    explicit ArrayTypeSpec(const Keyword *keyword) : TypeSpec(Type::ArrayTypeSpec), keyword(keyword) {}

};


struct IdentList : public Node {

    std::vector<const Identifier *> idents;


    IdentList() : Node(Type::IdentList) {}

};


struct ParamSpec : public Node {

    const Keyword *keyword_var = nullptr;

    IdentList *id_list;

    TypeSpec *spec;

    ParamSpec(const Keyword *keyword_var, IdentList *id_list, TypeSpec *spec) : Node(Type::ParamSpec),
                                                                                keyword_var(keyword_var),
                                                                                id_list(id_list), spec(spec) {}

};

struct ParamList : public Node {

    std::vector<ParamSpec *> params;


    ParamList() : Node(Type::ParamList) {}


    ~ParamList() {

        for (auto exp : params) {

            deleteAST(exp);

        }

    }

};


struct ExpressionList : public Node {
    std::vector<Exp *> explist;

    explicit ExpressionList() : Node(Type::ExpressionList) {}
};


struct Variable : public Exp {

    const Identifier *id = nullptr;

    ExpressionList *id_var = nullptr;

    explicit Variable() : Exp(Type::Variabele) {}
};


struct VariableList : public Node {

    std::vector<Variable *> params;


    VariableList() : Node(Type::VariableList) {}


    ~VariableList() {

        for (auto exp : params) {

            deleteAST(exp);

        }

    }

};


struct ConstDecl : public Node {

    const Identifier *ident;

    Exp *rhs;


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


struct Ident : public Exp {

    const Identifier *ident;


    explicit Ident(const Identifier *ident) : Exp(Type::Ident), ident(ident) {}

};


struct ExpAssign : public Exp {

    Exp *lhs, *rhs;


    ExpAssign(Exp *lhs, Exp *rhs) : Exp(Type::ExpAssign), lhs(lhs), rhs(rhs) {}

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

    ExpressionList *params;


    explicit ExpCall(const Identifier *fn, ExpressionList *params) : Exp(Type::ExpCall), fn(fn), params(params) {}

    ~ExpCall() {

        deleteAST(params);

    };

};


struct Statement : public Exp {

    explicit Statement(Type type) : Exp(type) {}

};


struct ExecStatement : public Statement {
    Exp *exp;

    explicit ExecStatement(Exp *exp) : Statement(Type::ExecStatement), exp(exp) {}

    ~ExecStatement() {
        deleteAST(exp);
    }
};

struct StatementList : public Node {

    std::vector<Statement *> statement;

    explicit StatementList() : Node(Type::StatementList) {}

};


struct Read : public Exp {
    VariableList *var_list = nullptr;

    explicit Read() : Exp(Type::Read) {}
};


struct Write : public Exp {
    ExpressionList *exp_list = nullptr;

    explicit Write() : Exp(Type::Write) {}
};


//struct StatementBlock : public Statement {
//
//  StatementBlock() : Statement(Type::StatementBlock) {}
//
//};



struct IfElseStatement : public Statement {

    Exp *expression = nullptr;

    Statement *if_part = nullptr;

    Statement *else_part = nullptr;

    IfElseStatement() : Statement(Type::IfElseStatement) {}

};


struct ForStatement : public Statement {

    const Identifier *id = nullptr;

    Exp *express1 = nullptr;

    Exp *express2 = nullptr;

    Statement *for_stmt = nullptr;

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


struct ExpMarker : public Exp {
    const Marker *value;

    explicit ExpMarker(const Marker *value) : Exp(Type::ExpMarker), value(value) {}

};

struct ExpKeyword : public Exp {
    const Keyword *value;

    explicit ExpKeyword(const Keyword *value)
            : Exp(Type::ExpKeyword), value(value) {}

};


struct ExpVoid : public Exp {

    explicit ExpVoid() : Exp(Type::ExpVoid) {}

};


struct CompoundStatement : public Statement {

    StatementList *state = nullptr;

    explicit CompoundStatement() : Statement(Type::CompoundStatement) {}

    explicit CompoundStatement(StatementList *state) : Statement(Type::CompoundStatement), state(state) {}
};


struct SubprogramHead : public Node { // subprogram head

    const Keyword *fn_def = nullptr;

    const Identifier *name = nullptr;

    ParamList *decls = nullptr;

    BasicTypeSpec *ret_type = nullptr;

    SubprogramHead() : Node(Type::SubprogramHead) {}

    SubprogramHead(const Keyword *fn_def, const Identifier *name, ParamList *decls, BasicTypeSpec *ret_type) :
            Node(Type::SubprogramHead), fn_def(fn_def), name(name), decls(decls), ret_type(ret_type) {}

    ~SubprogramHead() {
        deleteAST(decls);
        deleteAST(ret_type);

    }
};

struct SubprogramBody : public Node { // subprogram body

    ConstDecls *constdecls = nullptr;

    VarDecls *vardecls = nullptr;

    CompoundStatement *compound = nullptr;

    explicit SubprogramBody() : Node(Type::SubprogramBody) {}

    explicit SubprogramBody(ConstDecls *constdecls, VarDecls *vardecls, CompoundStatement *compound) :
            Node(Type::SubprogramBody), constdecls(constdecls), vardecls(vardecls), compound(compound) {}
};


struct Subprogram : public Function { // subprogram

    SubprogramHead *subhead;

    const Marker *semicolon = nullptr;

    SubprogramBody *subbody;

    explicit Subprogram(SubprogramHead *subhead, SubprogramBody *subbody) : Function(Type::Subprogram),
                                                                            subhead(subhead), subbody(subbody) {}
};


struct SubprogramDecls : public Node { // subprogram declarations

    std::vector<Subprogram *> subprogram;

    explicit SubprogramDecls() : Node(Type::SubprogramDecls) {}
};


struct ProgramHead : public Node {

    const ExpKeyword *programKeyword;

    Ident *id;

    IdentList *idlist;

    explicit ProgramHead(const ExpKeyword *programKeyword, Ident *id, IdentList *idlist) : Node(Type::ProgramHead),
                                                                                           programKeyword(
                                                                                                   programKeyword),
                                                                                           id(id), idlist(idlist) {}

};


struct ProgramBody : public Node {

    ConstDecls *constdecls;

    VarDecls *vardecls;

    SubprogramDecls *subprogram;

    CompoundStatement *compound;

    explicit ProgramBody(ConstDecls *constdecls, VarDecls *vardecls, SubprogramDecls *subprogram,
                         CompoundStatement *compound) :
            Node(Type::ProgramBody), constdecls(constdecls), vardecls(vardecls), subprogram(subprogram),
            compound(compound) {}

};


struct Program : public Function {

    Node fn_type;

    ProgramHead *programHead;

    const Marker *semicolon;

    ProgramBody *programBody;

    const Marker *dot = nullptr;


    explicit Program(ProgramHead *programHead, ProgramBody *programBody)

            : Function(Type::Program), fn_type(Type::Program), programHead(programHead), programBody(programBody) {}


    ~Program() {
        deleteAST(programHead);
        deleteAST(programBody);

    }
};




#endif
 */