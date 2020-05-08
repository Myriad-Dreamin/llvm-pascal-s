//
// Created by kamiyoru on 2020/5/8.
//

#include <pascal-s/mock.h>
#include <pascal-s/parser.h>

template struct Parser<MockLexer>;

#define expected_enum_type(predicator, indicate) do {if (!predicator(current_token)) {\
    ErrorList.push_back(new PascalSParseExpectGotError(__FUNCTION__, &indicate, current_token));\
    return nullptr;\
}}while(0)

#define expected_type_r(tok_type, rvalue) do {if (current_token == nullptr || current_token->type != tok_type) {\
    ErrorList.push_back(new PascalSParseExpectTGotError(__FUNCTION__, tok_type, current_token));\
    return rvalue;\
}}while(0)

#define expected_type(tok_type) expected_type_r(tok_type, nullptr)

template<typename Lexer>
Parser<Lexer>::Parser(LexerProxy<Lexer> lexer) : lexer(std::move(lexer)) {}

template<typename Lexer>
ast::Node *Parser<Lexer>::parse() {
    next_token();
    return parse_program_struct();
}

template<typename Lexer>
ast::Program *Parser<Lexer>::parse_program_struct() {
    auto program = current_token;
    expected_enum_type(predicate::is_program, predicate::keyword_program);
    auto ident = next_token();
    expected_type(TokenType::Identifier);
    next_token();
    if (!predicate::is_semicolon(current_token)) {
        parse_id_list_with_paren();
    }
    expected_enum_type(predicate::is_semicolon, predicate::marker_semicolon);
    next_token();

    ast::ConstDecls* const_decls = nullptr;
    if (predicate::is_const(current_token)) {
        const_decls = parse_const_decls();
    }

    return new ast::Program(
            reinterpret_cast<const Keyword*>(program),
            reinterpret_cast<const Identifier*>(ident),
            const_decls);
}

template<typename Lexer>
const Token *Parser<Lexer>::next_token() {
    current_token = lexer.next_token();
    return current_token;
}

template<typename Lexer>
ast::IdentList *Parser<Lexer>::parse_id_list_with_paren() {
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    next_token();
    parse_id_list();
    expected_enum_type(predicate::is_rparen, predicate::marker_rparen);
    next_token();
}

template<typename Lexer>
ast::IdentList *Parser<Lexer>::parse_id_list() {
    return _parse_id_list(new ast::IdentList);
}

template<typename Lexer>
ast::IdentList *Parser<Lexer>::_parse_id_list(ast::IdentList *params) {
    expected_type_r(TokenType::Identifier, params);
    params->idents.push_back(reinterpret_cast<const Identifier*>(current_token));
    next_token();
    if (predicate::is_comma(current_token)) {
        next_token();
        _parse_id_list(params);
    }
    return params;
}

template<typename Lexer>
ast::ConstDecls *Parser<Lexer>::parse_const_decls() {
    expected_enum_type(predicate::is_const, predicate::keyword_const);
    next_token();
    expected_type_r(TokenType::Identifier, nullptr);
    return _parse_const_decls(new ast::ConstDecls);
}

template<typename Lexer>
ast::ConstDecl *Parser<Lexer>::parse_const_decl() {
    expected_type_r(TokenType::Identifier, nullptr);
    auto ident = reinterpret_cast<const Identifier*>(current_token);
    next_token();
    expected_enum_type(predicate::is_eq, predicate::marker_eq);
    next_token();
    auto decl = new ast::ConstDecl(ident, parse_const_exp());
    expected_enum_type(predicate::is_semicolon, predicate::marker_semicolon);
    return decl;
}



template<typename Lexer>
ast::ConstDecls *Parser<Lexer>::_parse_const_decls(ast::ConstDecls *decls) {
    if (current_token == nullptr || current_token->type != TokenType::Identifier) {
        return decls;
    }

    auto decl = parse_const_decl();
    if (decl == nullptr) {
        return decls;
    }

    decls->decls.push_back(decl);
    return _parse_const_decls(decls);
}

template<typename Lexer>
ast::Exp *Parser<Lexer>::parse_const_exp() {

    if (current_token == nullptr) {
        ErrorList.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "const value", current_token));
        return nullptr;
    }
    auto rhs = current_token;
    ast::Exp * exp = nullptr;
    if (current_token->type == TokenType::ConstantInteger) {
        exp =  new ast::ExpConstantInteger(
                reinterpret_cast<const ConstantInteger*>(current_token));
    }
    if (current_token->type == TokenType::ConstantChar) {
        exp =  new ast::ExpConstantChar(
                reinterpret_cast<const ConstantChar*>(current_token));
    }
    if (current_token->type == TokenType::ConstantString) {
        exp =  new ast::ExpConstantString(
                reinterpret_cast<const ConstantString*>(current_token));
    }
    if (current_token->type == TokenType::ConstantReal) {
        exp =  new ast::ExpConstantReal(
                reinterpret_cast<const ConstantReal*>(current_token));
    }
    if (current_token->type == TokenType::ConstantBoolean) {
        exp =  new ast::ExpConstantBoolean(
                reinterpret_cast<const ConstantBoolean*>(current_token));
    }
    if (exp == nullptr) {
        return nullptr;
    }
    next_token();
    return exp;
}

template<typename Lexer>
ast::VarDecls *Parser<Lexer>::parse_var_decls() {
    expected_enum_type(predicate::is_var, predicate::keyword_var);
    next_token();
    expected_type_r(TokenType::Identifier, nullptr);
    return _parse_var_decls(new ast::VarDecls);
}

template<typename Lexer>
ast::VarDecls *Parser<Lexer>::_parse_var_decls(ast::VarDecls *decls) {
    if (current_token == nullptr || current_token->type != TokenType::Identifier) {
        return decls;
    }

    auto decl = parse_var_decl();
    if (decl == nullptr) {
        return decls;
    }

    decls->decls.push_back(decl);
    return _parse_var_decls(decls);
}

template<typename Lexer>
ast::VarDecl *Parser<Lexer>::parse_var_decl() {
    auto id_list = parse_id_list();
    expected_enum_type(predicate::is_colon, predicate::marker_colon);
    next_token();
    auto type = parse_type();
    expected_enum_type(predicate::is_semicolon, predicate::marker_semicolon);
    next_token();
    return new ast::VarDecl(id_list, type);
}

template<typename Lexer>
ast::TypeSpec *Parser<Lexer>::parse_type() {

    if (current_token == nullptr || current_token->type != TokenType::Keyword) {
        ErrorList.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "type spec", current_token));
        return nullptr;
    }

    auto array_or_basic = reinterpret_cast<const Keyword*>(current_token);
    if (array_or_basic->key_type == KeywordType::Array) {
        next_token();
        return parse_array_type(array_or_basic);
    }
    if (
            array_or_basic->key_type == KeywordType::Integer ||
                    array_or_basic->key_type == KeywordType::Real||
                    array_or_basic->key_type == KeywordType::Char||
                    array_or_basic->key_type == KeywordType::Boolean
            ) {
        next_token();
        return new ast::BasicTypeSpec(array_or_basic);
    } else {
        ErrorList.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", array_or_basic));
    }
    return nullptr;
}

template<typename Lexer>
ast::ArrayTypeSpec *Parser<Lexer>::parse_array_type(const Keyword *keyword_array) {
    expected_enum_type(predicate::is_lbracket, predicate::marker_lbracket);
    next_token();
    //todo: parse period
    expected_enum_type(predicate::is_rbracket, predicate::marker_rbracket);
    next_token();
    expected_enum_type(predicate::is_of, predicate::keyword_of);
    next_token();
    auto basic = reinterpret_cast<const Keyword*>(current_token);
    if (
            basic->key_type == KeywordType::Integer ||
            basic->key_type == KeywordType::Real||
            basic->key_type == KeywordType::Char||
            basic->key_type == KeywordType::Boolean
            ) {
        next_token();
        return new ast::ArrayTypeSpec(basic);
    } else {
        ErrorList.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", basic));
    }
    return nullptr;
}

template<typename Lexer>
ast::Exp *Parser<Lexer>::parse_exp() {
    return nullptr;
}

template<typename Lexer>
ast::FunctionDecls *Parser<Lexer>::parse_function_decls() {
    return nullptr;
}

template<typename Lexer>
ast::FunctionDecl *Parser<Lexer>::parse_function_decl() {
    return nullptr;
}

template<typename Lexer>
ast::ParamList *Parser<Lexer>::parse_param_list_with_paren() {
    return nullptr;
}

template<typename Lexer>
ast::ParamList *Parser<Lexer>::parse_param_list() {
    return nullptr;
}

template<typename Lexer>
ast::Statement *Parser<Lexer>::parse_statement() {
    return nullptr;
}

template<typename Lexer>
ast::VariableList *Parser<Lexer>::parse_variable_list_with_paren() {
    return nullptr;
}


template<typename Lexer>
ast::VariableList *Parser<Lexer>::parse_variable_list() {
    return nullptr;
}
#undef expected_keyword