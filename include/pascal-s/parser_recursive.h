//
// Created by kamiyoru on 2020/7/3.
//

#ifndef PASCAL_S_PARSER_RECURSIVE_H
#define PASCAL_S_PARSER_RECURSIVE_H

#include <pascal-s/interface.h>
#include <pascal-s/llvm-ast.h>

#include <vector>
#include <set>
#include <pascal-s/lib/guess.h>
#include "interface.h"
#include "llvm-ast.h"
#include "exception.h"

#include "parser.h"

template<typename Lexer>
class RecursiveParser : public Parser {
    LexerProxy<Lexer> lexer;
    std::vector<Token *> guessing_token;

public:
    const Token *current_token;
    std::vector<PascalSError *> errors;

    explicit RecursiveParser(LexerProxy<Lexer> lexer);

    virtual ~RecursiveParser() override;

    ast::Node *parse();

    const Token *next_token();

    ast::Program *parse_program();

    ast::ProgramHead *parse_program_head();

    ast::ProgramBody *parse_program_body();

    ast::IdentList *parse_id_list_with_paren();

    ast::ParamList *parse_param_list_with_paren();

    ast::ExpressionList *parse_expression_list_with_paren();

    ast::ExpressionList *parse_expression_list_with_bracket();

    ast::VariableList *parse_variable_list_with_paren();

    ast::IdentList *parse_id_list();

    ast::ParamList *parse_param_list();

    template<typename F>
    ast::ExpressionList *parse_expression_list(const F &is_follow, const std::set<const Token *> *till = nullptr);

    ast::VariableList *parse_variable_list();

    ast::ParamSpec *parse_param();

    ast::ConstDecls *parse_const_decls();

    ast::ConstDecl *parse_const_decl();

    ast::VarDecls *parse_var_decls();

    ast::VarDecl *parse_var_decl();

    ast::SubprogramDecls *parse_subprogram_decls();

    ast::SubprogramHead *parse_subprogram_head();

    ast::SubprogramBody *parse_subprogram_body();

    ast::TypeSpec *parse_type();

    ast::ArrayTypeSpec *parse_array_type(const Keyword *keyword_array);

    ast::Statement *parse_statement(std::set<const Token *> *till = nullptr);

    ast::CompoundStatement *parse_compound_statement(std::set<const Token *> *till = nullptr);

    ast::Statement *parse_for_statement(std::set<const Token *> *till = nullptr);

    ast::Statement *parse_if_else_statement(std::set<const Token *> *till = nullptr);

    ast::Statement *parse_read_statement(std::set<const Token *> *till = nullptr);

    ast::Statement *parse_write_statement(std::set<const Token *> *till = nullptr);

    ast::Exp *parse_const_exp(const std::set<const Token *> *till = nullptr);

    ast::Exp *parse_const_fac(const std::set<const Token *> *till = nullptr);

    ast::Exp *parse_exp(const std::set<const Token *> *till = nullptr);

    ast::Variable *parse_variable(const std::set<const Token *> *till = nullptr);

    ast::Exp *
    parse_binary_exp(ast::Exp *lhs, const Marker *marker, pascal_s::marker_type_underlying_type current_marker_pri,
                     const std::set<const Token *> *till = nullptr);

    ast::Exp *parse_fac(const std::set<const Token *> *till = nullptr);

    [[maybe_unused]] virtual bool has_error();

    [[maybe_unused]] const std::vector<PascalSError *> &get_all_errors() {
        return errors;
    }

private:

    pascal_s::MinStrDist guesser;

    ast::IdentList *_parse_id_list(ast::IdentList *);

    ast::ParamList *_parse_param_list(ast::ParamList *params);

    ast::ConstDecls *_parse_const_decls(ast::ConstDecls *);

    ast::VarDecls *_parse_var_decls(ast::VarDecls *);

    ast::SubprogramDecls *_parse_subprogram_decls(ast::SubprogramDecls *decls);

    ast::CompoundStatement *_parse_compound_statement(std::set<const Token *> *till = nullptr);

    ast::CompoundStatement *__parse_compound_statement(std::set<const Token *> *till = nullptr);

    bool guess_keyword(KeywordType k);

    void update_guess(Token *new_tok);
};

#endif //PASCAL_S_PARSER_RECURSIVE_H
