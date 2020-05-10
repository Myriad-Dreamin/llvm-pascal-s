//
// Created by kamiyoru on 2020/5/8.
//

#include <pascal-s/mock.h>
#include <pascal-s/parser.h>
#include <fmt/core.h>
#include <set>

//#ifdef WITH_MOCK
template
struct Parser<MockLexer>;
//#endif

#ifdef WITH_PASCAL_LEXER_FILES
#include <pascal-s/lexer.h>
template
struct Parser<FullInMemoryLexer>;
#endif

#define expected_enum_type(predicator, indicate) do {if (!predicator(current_token)) {\
    errors.push_back(new PascalSParseExpectGotError(__FUNCTION__, &indicate, current_token));\
    return nullptr;\
}}while(0)

#define expected_enum_type_r(predicator, indicate, rvalue) do {if (!predicator(current_token)) {\
    errors.push_back(new PascalSParseExpectGotError(__FUNCTION__, &indicate, current_token));\
    return rvalue;\
}}while(0)

#define expected_type_r(tok_type, rvalue) do {if (current_token == nullptr || current_token->type != tok_type) {\
    errors.push_back(new PascalSParseExpectTGotError(__FUNCTION__, tok_type, current_token));\
    return rvalue;\
}}while(0)

#define expected_type(tok_type) expected_type_r(tok_type, nullptr)

namespace predicate {
    struct _parserPredicateContainer {
        const std::set<const Token *> rParenContainer;
        const std::set<const Token *> semicolonContainer;
        const std::vector<Token *> thenContainer;
        const std::vector<Token *> elseContainer;
        const std::vector<Token *> endOrSemicolonContainer;
        const std::set<const Token *> commaOrRParenContainer;
        const std::vector<Token *> endOrSemicolonOrElseContainer;


        explicit _parserPredicateContainer() noexcept:
                rParenContainer(
                        {
                                reinterpret_cast<const Token *>(&predicate::marker_rparen)
                        }),
                semicolonContainer(
                        {
                                reinterpret_cast<const Token *>(&predicate::marker_semicolon)
                        }),
                thenContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_then))
                        }),
                elseContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_else))
                        }),
                endOrSemicolonContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_semicolon)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_end)),
                        }),
                commaOrRParenContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_comma)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_rparen)),
                        }),
                endOrSemicolonOrElseContainer(
                        {
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_comma)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::marker_rparen)),
                                const_cast<Token *>(reinterpret_cast<const Token *>(&predicate::keyword_else)),
                        }) {
        }
    } predicateContainers;
}

template<typename Lexer>
Parser<Lexer>::Parser(LexerProxy<Lexer> lexer) : lexer(std::move(lexer)) {}

template<typename Lexer>
const Token *Parser<Lexer>::next_token() {
    current_token = lexer.next_token();
    return current_token;
}

template<typename Lexer>
ast::Node *Parser<Lexer>::parse() {
    next_token();
    return parse_program_struct();
}

template<typename Lexer>
ast::Program *Parser<Lexer>::parse_program_struct() {

    // program id
    auto program = reinterpret_cast<const Keyword *>(current_token);
    expected_enum_type(predicate::is_program, predicate::keyword_program);
    auto ident = reinterpret_cast<const Identifier *>(next_token());
    expected_type(TokenType::Identifier);

    // maybe ( id list )
    next_token();
    ast::IdentList *ident_list = nullptr;
    if (!predicate::is_semicolon(current_token)) {
        ident_list = parse_id_list_with_paren();
    }

    // ;
    expected_enum_type(predicate::is_semicolon, predicate::marker_semicolon);
    next_token();

    // const declarations
    ast::ConstDecls *const_decls = nullptr;
    if (predicate::is_const(current_token)) {
        const_decls = parse_const_decls();
    }

    // var declarations
    ast::VarDecls *var_decls = nullptr;
    if (predicate::is_var(current_token)) {
        var_decls = parse_var_decls();
    }

    // function declarations
    ast::FunctionDecls *fn_decls = nullptr;
    if (predicate::is_function(current_token) || predicate::is_procedure(current_token)) {
        fn_decls = parse_function_decls();
    }

    std::set<const Token *> till;
    till.insert(reinterpret_cast<const Token *>(&predicate::marker_semicolon));
    // program statement
    return new ast::Program(
            program, ident, ident_list,
            const_decls, var_decls, fn_decls, parse_statement(&till));
}

template<typename Lexer>
ast::IdentList *Parser<Lexer>::parse_id_list_with_paren() {

    // lparen
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    next_token();

    // id list
    auto ident_list = parse_id_list();
    if (ident_list == nullptr) {
        return nullptr;
    }

    // rparen
    if (!predicate::is_rparen(current_token)) {
        delete ident_list;
        errors.push_back(new PascalSParseExpectGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    next_token();

    return ident_list;
}

template<typename Lexer>
ast::IdentList *Parser<Lexer>::parse_id_list() {
    // allocate
    return _parse_id_list(new ast::IdentList);
}

template<typename Lexer>
ast::IdentList *Parser<Lexer>::_parse_id_list(ast::IdentList *params) {

    // id
    expected_type_r(TokenType::Identifier, params);
    params->idents.push_back(reinterpret_cast<const Identifier *>(current_token));
    next_token();

    // ,
    if (predicate::is_comma(current_token)) {
        next_token();

        // id list
        _parse_id_list(params);
    }
    return params;
}

template<typename Lexer>
ast::ConstDecls *Parser<Lexer>::parse_const_decls() {

    // const
    expected_enum_type(predicate::is_const, predicate::keyword_const);
    next_token();

    // look ahead
    expected_type_r(TokenType::Identifier, nullptr);

    // declarations
    return _parse_const_decls(new ast::ConstDecls);
}

template<typename Lexer>
ast::ConstDecls *Parser<Lexer>::_parse_const_decls(ast::ConstDecls *decls) {

    // declaration
    auto decl = parse_const_decl();
    if (decl == nullptr) {
        return decls;
    }
    decls->decls.push_back(decl);

    // look ahead
    if (current_token == nullptr || current_token->type != TokenType::Identifier) {
        return decls;
    }
    return _parse_const_decls(decls);
}

template<typename Lexer>
ast::ConstDecl *Parser<Lexer>::parse_const_decl() {
    // id
    expected_type_r(TokenType::Identifier, nullptr);
    auto ident = reinterpret_cast<const Identifier *>(current_token);
    next_token();

    // =
    expected_enum_type(predicate::is_eq, predicate::marker_eq);
    next_token();

    // const exp
    auto decl = new ast::ConstDecl(ident, parse_const_exp(&predicate::predicateContainers.semicolonContainer));

    // ;
    expected_enum_type(predicate::is_semicolon, predicate::marker_semicolon);
    next_token();

    return decl;
}

template<typename Lexer>
ast::Exp *Parser<Lexer>::parse_const_exp(const std::set<const Token *> *till) {

    auto *maybe_lhs = parse_const_fac(till);
    if (maybe_lhs == nullptr) {
        return nullptr;
    }

    if (predicate::token_equal(current_token, till)) {
        next_token();
        return maybe_lhs;
    }

    assert(false);
    // todo: parse recursive const exp
    return nullptr;
}

template<typename Lexer>
ast::Exp *Parser<Lexer>::parse_const_fac(const std::set<const Token *> *till) {
    if (current_token == nullptr) {
        errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "const value", current_token));
        return nullptr;
    }

    if (current_token->type == TokenType::Marker) {
        auto marker = reinterpret_cast<const Marker *>(current_token);
        switch (marker->marker_type) {
            case MarkerType::Add:
            case MarkerType::Sub:
                next_token();
                return new ast::UnExp(marker, parse_const_exp(till));
            case MarkerType::LParen:
                next_token();
                return parse_const_exp(&predicate::predicateContainers.rParenContainer);
            default:
                throw std::runtime_error("expected +/-/(");
        }
    }

    // parse const fac
    ast::Exp *fac = nullptr;
    if (current_token->type == TokenType::ConstantInteger) {
        fac = new ast::ExpConstantInteger(
                reinterpret_cast<const ConstantInteger *>(current_token));
    }
    if (current_token->type == TokenType::ConstantChar) {
        fac = new ast::ExpConstantChar(
                reinterpret_cast<const ConstantChar *>(current_token));
    }
    if (current_token->type == TokenType::ConstantString) {
        fac = new ast::ExpConstantString(
                reinterpret_cast<const ConstantString *>(current_token));
    }
    if (current_token->type == TokenType::ConstantReal) {
        fac = new ast::ExpConstantReal(
                reinterpret_cast<const ConstantReal *>(current_token));
    }
    if (current_token->type == TokenType::ConstantBoolean) {
        fac = new ast::ExpConstantBoolean(
                reinterpret_cast<const ConstantBoolean *>(current_token));
    }
    if (fac == nullptr) {
        return nullptr;
    }

    next_token();

    return fac;
}

template<typename Lexer>
ast::VarDecls *Parser<Lexer>::parse_var_decls() {

    //var
    expected_enum_type(predicate::is_var, predicate::keyword_var);
    next_token();

    // look ahead
    expected_type_r(TokenType::Identifier, nullptr);
    return _parse_var_decls(new ast::VarDecls);
}

template<typename Lexer>
ast::VarDecls *Parser<Lexer>::_parse_var_decls(ast::VarDecls *decls) {

    // declaration
    auto decl = parse_var_decl();
    if (decl == nullptr) {
        return decls;
    }
    decls->decls.push_back(decl);

    // look ahead
    if (current_token == nullptr || current_token->type != TokenType::Identifier) {
        return decls;
    }
    return _parse_var_decls(decls);
}

template<typename Lexer>
ast::VarDecl *Parser<Lexer>::parse_var_decl() {

    // id list
    auto id_list = parse_id_list();
    if (id_list == nullptr) {
        return nullptr;
    }

    // :
    expected_enum_type(predicate::is_colon, predicate::marker_colon);
    next_token();

    // type
    auto type = parse_type();
    if (type == nullptr) {
        return nullptr;
    }

    // ;
    expected_enum_type(predicate::is_semicolon, predicate::marker_semicolon);
    next_token();


    return new ast::VarDecl(id_list, type);
}

template<typename Lexer>
ast::TypeSpec *Parser<Lexer>::parse_type() {
    // next token == array | basic_type, all belong to keyword
    if (current_token == nullptr || current_token->type != TokenType::Keyword) {
        errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "type spec", current_token));
        return nullptr;
    }


    auto array_or_basic = reinterpret_cast<const Keyword *>(current_token);

    // array
    if (array_or_basic->key_type == KeywordType::Array) {
        next_token();
        return parse_array_type(array_or_basic);
    } else if (
        // basic type
            array_or_basic->key_type == KeywordType::Integer ||
            array_or_basic->key_type == KeywordType::Real ||
            array_or_basic->key_type == KeywordType::Char ||
            array_or_basic->key_type == KeywordType::Boolean
            ) {
        next_token();
        return new ast::BasicTypeSpec(array_or_basic);
    }
    errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", array_or_basic));
    return nullptr;
}

template<typename Lexer>
ast::ArrayTypeSpec *Parser<Lexer>::parse_array_type(const Keyword *keyword_array) {

    // [
    expected_enum_type(predicate::is_lbracket, predicate::marker_lbracket);
    next_token();

    expected_enum_type(predicate::is_integer, predicate::keyword_integer);
    std::vector<std::pair<const ConstantInteger *, const ConstantInteger *>> periods;
    // digit
    while (predicate::is_integer(current_token)) {
        auto l_range = reinterpret_cast<const ConstantInteger *>(current_token);

        // ..
        expected_enum_type(predicate::is_range, predicate::marker_range);
        next_token();

        // digit
        expected_enum_type(predicate::is_integer, predicate::keyword_integer);
        periods.emplace_back(l_range, reinterpret_cast<const ConstantInteger *>(current_token));
    }

    // ]
    expected_enum_type(predicate::is_rbracket, predicate::marker_rbracket);
    next_token();

    // of
    expected_enum_type(predicate::is_of, predicate::keyword_of);
    next_token();

    // basic_type
    auto basic = reinterpret_cast<const Keyword *>(current_token);
    if (
            basic->key_type == KeywordType::Integer ||
            basic->key_type == KeywordType::Real ||
            basic->key_type == KeywordType::Char ||
            basic->key_type == KeywordType::Boolean
            ) {
        next_token();
        return new ast::ArrayTypeSpec(periods, basic);
    }

    errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", basic));
    return nullptr;
}

template<typename Lexer>
ast::FunctionDecls *Parser<Lexer>::parse_function_decls() {

    // look ahead
    if (predicate::is_function(current_token) || predicate::is_procedure(current_token)) {

        // procedure or function
        return _parse_function_decls(new ast::FunctionDecls);
    }
    return nullptr;
}

template<typename Lexer>
ast::FunctionDecls *Parser<Lexer>::_parse_function_decls(ast::FunctionDecls *decls) {

    // function head
    ast::Procedure *proc = parse_function_head();
    if (proc == nullptr) {
        return decls;
    }

    // function should with type
    if (predicate::is_function(proc->fn_def)) {

        // :
        expected_enum_type_r(predicate::is_colon, predicate::marker_colon, decls);
        next_token();

        // basic type
        auto basic = reinterpret_cast<const Keyword *>(current_token);
        if (
                basic->key_type == KeywordType::Integer ||
                basic->key_type == KeywordType::Real ||
                basic->key_type == KeywordType::Char ||
                basic->key_type == KeywordType::Boolean
                ) {
            next_token();
            proc->fn_def = basic;
        } else {
            errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", basic));
            delete proc;
            return decls;
        }
    }

    // ;
    expected_enum_type_r(predicate::is_semicolon, predicate::marker_semicolon, decls);
    next_token();

    // function body
    proc = parse_function_body(proc);
    if (proc != nullptr) {
        decls->decls.push_back(proc);
    }

    // look ahead
    if (predicate::is_function(current_token) || predicate::is_procedure(current_token)) {
        return _parse_function_decls(decls);
    }

    return decls;
}

template<typename Lexer>
ast::Procedure *Parser<Lexer>::parse_function_head() {
    // keyword procedure or function
    auto fn_def = reinterpret_cast<const Keyword *>(current_token);

    // id
    auto ident = reinterpret_cast<const Identifier *>(next_token());
    expected_type(TokenType::Identifier);
    next_token();

    // share common struct type Procedure
    auto proc = new ast::Procedure(fn_def, ident);

    // param list
    if (predicate::is_lparen(current_token)) {
        proc->params = parse_param_list_with_paren();
        if (proc->params == nullptr) {
            delete proc;
            return nullptr;
        }
    }

    return proc;
}


template<typename Lexer>
ast::Procedure *Parser<Lexer>::parse_function_body(ast::Procedure *proc) {

    // const declarations
    ast::ConstDecls *const_decls = nullptr;
    if (predicate::is_const(current_token)) {
        proc->const_decls = parse_const_decls();
    }

    // var declarations
    if (predicate::is_var(current_token)) {
        proc->var_decls = parse_var_decls();
    }

    // statement
    proc->body = parse_statement();
    return proc;
}

template<typename Lexer>
ast::ParamList *Parser<Lexer>::parse_param_list_with_paren() {

    // (
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    next_token();

    // param list
    auto param_list = parse_param_list();
    if (param_list == nullptr) {
        return nullptr;
    }

    // )
    if (!predicate::is_rparen(current_token)) {
        delete param_list;
        errors.push_back(new PascalSParseExpectGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    next_token();

    return param_list;
}

template<typename Lexer>
ast::ParamList *Parser<Lexer>::parse_param_list() {
    //look ahead
    if (current_token == nullptr || current_token->type != TokenType::Identifier) {
        return nullptr;
    }
    return _parse_param_list(new ast::ParamList);
}


template<typename Lexer>
ast::ParamList *Parser<Lexer>::_parse_param_list(ast::ParamList *params) {
    auto spec = parse_param();
    if (spec == nullptr) {
        return params;
    }
    params->params.push_back(spec);

    //look ahead
    if (current_token == nullptr || current_token->type != TokenType::Identifier) {

        // param list
        _parse_param_list(params);
    }
    return params;
}

template<typename Lexer>
ast::ParamSpec *Parser<Lexer>::parse_param() {
    auto keyword_var = reinterpret_cast<const Keyword *>(current_token);
    if (current_token == nullptr ||
        keyword_var->type != TokenType::Keyword ||
        keyword_var->key_type != KeywordType::Var) {

        keyword_var = nullptr;
    }

    // id list
    auto id_list = parse_id_list();
    if (id_list == nullptr) {
        return nullptr;
    }

    // :
    expected_enum_type(predicate::is_colon, predicate::marker_colon);
    next_token();

    auto basic = reinterpret_cast<const Keyword *>(current_token);
    if (basic == nullptr || basic->type != TokenType::Keyword) {
        return nullptr;
    }
    if (
        // basic type
            basic->key_type != KeywordType::Integer &&
            basic->key_type != KeywordType::Real &&
            basic->key_type != KeywordType::Char &&
            basic->key_type != KeywordType::Boolean
            ) {
        errors.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", basic));
        return nullptr;
    }
    next_token();

    // ;
    expected_enum_type(predicate::is_semicolon, predicate::marker_semicolon);
    next_token();

    return new ast::ParamSpec(keyword_var, id_list, new ast::BasicTypeSpec(basic));
}

template<typename Lexer>
ast::Statement *Parser<Lexer>::parse_statement(std::set<const Token *> *till) {

    // begin
    if (predicate::is_begin(current_token)) {
        auto begin_tok = reinterpret_cast<const Keyword *>(current_token);
        next_token();

        std::vector<ast::Statement *> stmts;

        // end
        ast::Statement *stmt;
        while (!predicate::is_end(current_token)) {
            if (current_token == nullptr) {
                throw std::invalid_argument("bad ... eof");
            }

            // statement
            if (till == nullptr) {
                std::set<const Token *> m_till;
                m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_end));
                m_till.insert(reinterpret_cast<const Token *>(&predicate::marker_semicolon));
                stmt = parse_statement(&m_till);
            } else {
                bool no_end = till->count(&predicate::keyword_end), no_semi = till->count(&predicate::marker_semicolon);
                if (no_end) {
                    till->insert(&predicate::keyword_end);
                }
                if (no_semi) {
                    till->insert(&predicate::marker_semicolon);
                }
                stmt = parse_statement(till);
                if (no_end) {
                    till->erase(&predicate::keyword_end);
                }
                if (no_semi) {
                    till->erase(&predicate::marker_semicolon);
                }
            }

            if (stmt != nullptr) {
                stmts.push_back(stmt);
            }

            // eat ; if possible
            if (predicate::is_semicolon(current_token)) {
                next_token();
            }
        }

        // end
        auto end_tok = reinterpret_cast<const Keyword *>(current_token);
        next_token();

        auto block = new ast::StatementBlock(begin_tok, end_tok);
        block->stmts.swap(stmts);
        return block;

        // assign op or func call
    } else if (current_token != nullptr && (current_token->type == TokenType::Identifier)) {

        // we extend the prod of statement to reduce work
        ast::Exp *exp = parse_exp(till);

        if (exp == nullptr) {
            return nullptr;
        } else {
            return new ast::ExecStatement(exp);
        }
    } else if (predicate::is_for(current_token)) {
        auto *for_stmt = new ast::ForStatement();

        // for
        expected_enum_type_r(predicate::is_for, predicate::keyword_for, for_stmt);
        next_token();

        // id
        expected_type_r(TokenType::Identifier, for_stmt);
        for_stmt->loop_var = reinterpret_cast<const Identifier *>(current_token);
        next_token();

        // :=
        expected_enum_type_r(predicate::is_assgin, predicate::marker_assgin, for_stmt);
        next_token();
        if (till == nullptr) {
            std::set<const Token *> m_till;
            m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_to));
            for_stmt->from_exp = parse_exp(&m_till);
        } else {
            bool no_to = till->count(&predicate::keyword_to);
            if (no_to) {
                till->insert(&predicate::keyword_to);
            }
            for_stmt->from_exp = parse_exp(till);
            if (no_to) {
                till->erase(&predicate::keyword_to);
            }
        }

        // to
        expected_enum_type_r(predicate::is_to, predicate::keyword_to, for_stmt);
        next_token();
        if (till == nullptr) {
            std::set<const Token *> m_till;
            m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_do));
            for_stmt->to_exp = parse_exp(&m_till);
        } else {
            bool no_do = till->count(&predicate::keyword_do);
            if (no_do) {
                till->insert(&predicate::keyword_do);
            }
            for_stmt->to_exp = parse_exp(till);
            if (no_do) {
                till->erase(&predicate::keyword_do);
            }
        }

        // do
        expected_enum_type_r(predicate::is_do, predicate::keyword_do, for_stmt);
        next_token();
        for_stmt->for_stmt = parse_statement(till);


        return for_stmt;
    } else if (predicate::is_if(current_token)) {
        auto *if_else = new ast::IfElseStatement();

        // if
        expected_enum_type_r(predicate::is_if, predicate::keyword_if, if_else);
        next_token();

        // if cond
        if (till == nullptr) {
            std::set<const Token *> m_till;
            m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_then));
            if_else->cond = parse_exp(&m_till);
        } else {
            bool no_then = till->count(&predicate::keyword_then);
            if (no_then) {
                till->insert(&predicate::keyword_then);
            }
            if_else->cond = parse_exp(till);
            if (no_then) {
                till->erase(&predicate::keyword_then);
            }
        }

        // then
        expected_enum_type_r(predicate::is_then, predicate::keyword_then, if_else);
        next_token();

        // if stmt
        if (till == nullptr) {
            std::set<const Token *> m_till;
            m_till.insert(reinterpret_cast<const Token *>(&predicate::keyword_else));
            if_else->if_stmt = parse_statement(&m_till);
        } else {
            bool no_else = till->count(&predicate::keyword_else);
            if (no_else) {
                till->insert(&predicate::keyword_else);
            }
            if_else->if_stmt = parse_statement(till);
            if (no_else) {
                till->erase(&predicate::keyword_else);
            }
        }

        // else part
        if (predicate::is_else(current_token)) {
            next_token();

            // else stmt
            if_else->else_stmt = parse_statement(till);
        }

        return if_else;
    }

    // epsilon
    return nullptr;
}


template<typename Lexer>
ast::VariableList *Parser<Lexer>::parse_variable_list_with_paren() {

    // (
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    next_token();

    // variable list
    auto var_list = parse_variable_list();
    if (var_list == nullptr) {
        return nullptr;
    }

    // )
    if (!predicate::is_rparen(current_token)) {
        delete var_list;
        errors.push_back(new PascalSParseExpectGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    next_token();

    return var_list;
}

template<typename Lexer>
ast::VariableList *Parser<Lexer>::parse_variable_list() {
    auto *ret = new ast::VariableList;
    for (;;) {

        // look ahead
        if (predicate::is_rparen(current_token)) {
            return ret;
        }

        // extend production
        ret->params.push_back(parse_exp(&predicate::predicateContainers.commaOrRParenContainer));

        // eat , if possible
        if (predicate::is_comma(current_token)) {
            next_token();

            // want FOLLOW(variable) = {)}
        } else if (!predicate::is_rparen(current_token)) {
            delete ret;
            throw std::runtime_error("expected ,/)");
            return nullptr;
        }
    }
    return nullptr;
}

template<typename Lexer>
ast::Exp *Parser<Lexer>::parse_exp(const std::set<const Token *> *till) {

    // lhs
    auto maybe_lhs = parse_fac();
    if (predicate::token_equal(current_token, till)) {
        next_token();
        return maybe_lhs;
    }

    if (current_token->type == TokenType::Marker) {
        auto marker = reinterpret_cast<const Marker *>(current_token);
        switch (marker->marker_type) {
            case MarkerType::Add:
            case MarkerType::Sub:
            case MarkerType::Mul:
            case MarkerType::Div:
//            case MarkerType::Mod:
                next_token();
                return parse_binary_exp(maybe_lhs, marker, get_marker_pri(marker->marker_type), till);
            case MarkerType::Dot:
            case MarkerType::RParen:
                return maybe_lhs;
            case MarkerType::Assign:
                next_token();
                return new ast::ExpAssign(maybe_lhs, parse_exp(till));
            default:
                assert(false);
                return nullptr;
        }
    } else {
        throw std::runtime_error(
                std::string("want marker, got ") + fmt::format("{}",
                                                               convertToString(current_token))
        );
    }

}

template<typename Lexer>
ast::Exp *Parser<Lexer>::parse_fac() {
    if (current_token == nullptr) {
        throw std::runtime_error("expected fac, got nullptr");
    }

    if (current_token->type == TokenType::ConstantInteger) {
        auto num = new ast::ExpConstantInteger(
                reinterpret_cast<const ConstantInteger *>(current_token));
        next_token();
        return num;
    } else if (current_token->type == TokenType::ConstantChar) {
        auto ch = new ast::ExpConstantChar(
                reinterpret_cast<const ConstantChar *>(current_token));
        next_token();
        return ch;
    } else if (current_token->type == TokenType::ConstantReal) {
        auto real = new ast::ExpConstantReal(
                reinterpret_cast<const ConstantReal *>(current_token));
        next_token();
        return real;
    } else if (current_token->type == TokenType::ConstantString) {
        auto str = new ast::ExpConstantString(
                reinterpret_cast<const ConstantString *>(current_token));
        next_token();
        return str;
    } else if (current_token->type == TokenType::ConstantBoolean) {
        auto bl = new ast::ExpConstantBoolean(
                reinterpret_cast<const ConstantBoolean *>(current_token));
        next_token();
        return bl;
    } else if (current_token->type == TokenType::Marker) {
        auto marker = reinterpret_cast<const Marker *>(current_token);
        switch (marker->marker_type) {
            case MarkerType::Add:
            case MarkerType::Sub:
                next_token();
                return new ast::UnExp(marker, parse_exp());
            case MarkerType::LParen:
                next_token();
                return parse_exp(&predicate::predicateContainers.rParenContainer);
            default:
                throw std::runtime_error("expected +/-/(");
        }

        // function call or array index or identifier exp
        // ident
    } else if (current_token->type == TokenType::Identifier) {
        auto ident = reinterpret_cast<const Identifier *>(current_token);
        next_token();

        // (
        if (current_token != nullptr && current_token->type == TokenType::Marker) {
            auto marker = reinterpret_cast<const Marker *>(current_token);
            if (marker->marker_type == MarkerType::LParen) {

                // will not eat (, just parse ( variable list )
                return new ast::ExpCall(
                        ident, parse_variable_list_with_paren());
                // [
            } else if (marker->marker_type == MarkerType::LBracket) {
                //todo
                throw std::runtime_error("todo");
            }
        }

        // otherwise just a identifier exp
        return new ast::Ident(ident);
    } else if (current_token->type == TokenType::Keyword) {
        //todo
        throw std::runtime_error("todo");
    } else {
        throw std::runtime_error("expected fac");
    }
}

template<typename Lexer>
ast::Exp *
Parser<Lexer>::parse_binary_exp(ast::Exp *lhs, const Marker *marker, marker_type_underlying_type current_marker_pri,
                                const std::set<const Token *> *till) {

    auto rhs = parse_fac();
    if (predicate::token_equal(current_token, till)) {
        next_token();
        return new ast::BiExp(lhs, marker, rhs);
    }
    if (current_token->type != TokenType::Marker) {
        throw std::runtime_error("expected marker");
    }
    auto pri = get_marker_pri(reinterpret_cast<const Marker *>(current_token)->marker_type);
    auto next_marker = reinterpret_cast<const Marker *>(current_token);
    next_token();
    if (current_marker_pri >= pri) {
        return parse_binary_exp(new ast::BiExp(lhs, marker, rhs),
                                next_marker, pri, till);
    }
    return new ast::BiExp(lhs, marker, parse_binary_exp(rhs, next_marker, pri, till));
}


#undef expected_keyword
#undef expected_enum_type
#undef expected_enum_type_r
#undef expected_type_r
#undef expected_type