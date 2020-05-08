//
// Created by kamiyoru on 2020/5/8.
//

#ifndef PASCAL_S_PARSER_H
#define PASCAL_S_PARSER_H

#include <vector>
#include "interface.h"
#include "ast.h"
#include "exception.h"

template<typename Lexer>
class Parser {
    LexerProxy<Lexer> lexer;
    const Token* current_token;

public:
    std::vector<PascalSError*> errors;

    explicit Parser(LexerProxy<Lexer> lexer);

    ast::Node *parse();

    ast::Program *parse_program_struct();

    ast::IdentList *parse_id_list_with_paren();
    ast::ParamList *parse_param_list_with_paren();
    ast::VariableList *parse_variable_list_with_paren();

    ast::IdentList *parse_id_list();
    ast::ParamList *parse_param_list();
    ast::VariableList *parse_variable_list();

    ast::ConstDecls *parse_const_decls();
    ast::ConstDecl *parse_const_decl();
    ast::VarDecls *parse_var_decls();
    ast::VarDecl *parse_var_decl();
    ast::Procedure *parse_function_head();
    ast::Procedure *parse_function_body(ast::Procedure*);
    ast::FunctionDecls *parse_function_decls();

    ast::TypeSpec *parse_type();
    ast::ArrayTypeSpec *parse_array_type(const Keyword* keyword_array);

    ast::Exp *parse_const_exp();
    ast::Exp *parse_exp(const std::vector<Token *> *till = nullptr);
    ast::Exp *parse_binary_exp(ast::Exp *lhs, const Marker *marker, marker_type_underlying_type current_marker_pri,
            const std::vector<Token *> *till = nullptr);
    ast::Exp *parse_fac();
    ast::Statement *parse_statement();

    const Token* next_token();

private:
    ast::IdentList *_parse_id_list(ast::IdentList *);
    ast::ConstDecls *_parse_const_decls(ast::ConstDecls *);
    ast::VarDecls *_parse_var_decls(ast::VarDecls *);
    ast::FunctionDecls *_parse_function_decls(ast::FunctionDecls *decls);
};


#endif //PASCAL_S_PARSER_H
