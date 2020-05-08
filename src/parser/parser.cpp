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

#define expected_enum_type_r(predicator, indicate, rvalue) do {if (!predicator(current_token)) {\
    ErrorList.push_back(new PascalSParseExpectGotError(__FUNCTION__, &indicate, current_token));\
    return rvalue;\
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
ast::Exp *Parser<Lexer>::parse_exp(const std::vector<Token *> *till) {
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
            case MarkerType::Dot: case MarkerType::RParen:
                return maybe_lhs;
            default:
                assert(false);
                return nullptr;
        }
    } else {
        throw std::runtime_error("want marker, got ?");
    }

}
const std::vector<Token*> rParenContainer({
                     const_cast<Token*>(reinterpret_cast<const Token*>(&predicate::marker_rparen))
             });

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
    } else if (current_token->type == TokenType::Marker) {
        auto marker = reinterpret_cast<const Marker *>(current_token);
        switch (marker->marker_type) {
            case MarkerType::Add:
            case MarkerType::Sub:
                next_token();
                return new ast::UnExp(marker, parse_exp());
            case MarkerType::LParen:
                next_token();
                return parse_exp(&rParenContainer);
            default:
                throw std::runtime_error("expected +/-/(");
        }
    } else if (current_token->type == TokenType::Identifier) {
        auto ident = reinterpret_cast<const Identifier *>(current_token);
        next_token();
        if (current_token != nullptr && current_token->type == TokenType::Marker) {
            auto marker = reinterpret_cast<const Marker *>(current_token);
            if (marker->marker_type == MarkerType::LParen) {
                next_token();
                return new ast::ExpCall(
                        ident, parse_variable_list());
            }
        }
        return new ast::Ident(ident);
    } else if (current_token->type == TokenType::Keyword) {
        throw std::runtime_error("todo");
    } else {
        throw std::runtime_error("expected fac");
    }
}

template<typename Lexer>
ast::Exp *Parser<Lexer>::parse_binary_exp(ast::Exp *lhs, const Marker *marker, marker_type_underlying_type current_marker_pri,
                const std::vector<Token *> *till) {

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


template<typename Lexer>
ast::FunctionDecls *Parser<Lexer>::parse_function_decls() {

    if (predicate::is_function(current_token) || predicate::is_procedure(current_token)) {
        return _parse_function_decls(new ast::FunctionDecls);
    }
    return nullptr;
}

template<typename Lexer>
ast::FunctionDecls*Parser<Lexer>::_parse_function_decls(ast::FunctionDecls*decls) {
    ast::Procedure *proc = parse_function_head();
    if (proc == nullptr) {
        return decls;
    }
    if (predicate::is_function(proc->fn_def)) {

        expected_enum_type_r(predicate::is_colon, predicate::marker_colon, decls);
        next_token();
        auto basic = reinterpret_cast<const Keyword*>(current_token);
        if (
                basic->key_type == KeywordType::Integer ||
                basic->key_type == KeywordType::Real||
                basic->key_type == KeywordType::Char||
                basic->key_type == KeywordType::Boolean
                ) {
            next_token();
            proc->fn_def = basic;
        } else {
            ErrorList.push_back(new PascalSParseExpectSGotError(__FUNCTION__, "basic type spec", basic));
            delete proc;
            return decls;
        }
    }
    expected_enum_type_r(predicate::is_colon, predicate::marker_colon, decls);
    next_token();

    proc = parse_function_body(proc);
    if (proc != nullptr) {
        decls->decls.push_back(proc);
    }

    if (predicate::is_function(current_token) || predicate::is_procedure(current_token)) {
        return _parse_function_decls(decls);
    }
    return decls;
}

template<typename Lexer>
ast::Procedure *Parser<Lexer>::parse_function_head() {
    auto fn_def = reinterpret_cast<const Keyword*>(current_token);

    auto ident = reinterpret_cast<const Identifier*>(next_token());
    expected_type(TokenType::Identifier);
    next_token();

    auto proc = new ast::Procedure(fn_def, ident);
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
ast::ParamList *Parser<Lexer>::parse_param_list() {
    return nullptr;
}

template<typename Lexer>
ast::VariableList *Parser<Lexer>::parse_variable_list() {
//    for (;;) {
//        if (predicate::token_equal(&rParen, current_token)) {
//            next_token();
//            return params;
//        }
//        params->push_back(parse_exp());
//        if (predicate::token_equal(&dot, current_token)) {
//            next_token();
//        } else if (!predicate::token_equal(&rParen, current_token)) {
//            throw std::runtime_error("expected ,/)");
//        }
//    }
    return nullptr;
}

template<typename Lexer>
ast::Statement *Parser<Lexer>::parse_statement() {
    return nullptr;
}


template<typename Lexer>
ast::Procedure *Parser<Lexer>::parse_function_body(ast::Procedure *) {
    return nullptr;
}

template<typename Lexer>
ast::IdentList *Parser<Lexer>::parse_id_list_with_paren() {
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    next_token();
    auto ident_list = parse_id_list();
    if (ident_list == nullptr) {
        return nullptr;
    }
    if (!predicate::is_rparen(current_token)) {
        delete ident_list;
        ErrorList.push_back(new PascalSParseExpectGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    next_token();
    return ident_list;
}

template<typename Lexer>
ast::ParamList *Parser<Lexer>::parse_param_list_with_paren() {
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    next_token();
    auto ident_list = parse_param_list();
    if (ident_list == nullptr) {
        return nullptr;
    }
    if (!predicate::is_rparen(current_token)) {
        delete ident_list;
        ErrorList.push_back(new PascalSParseExpectGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    next_token();
    return ident_list;
}


template<typename Lexer>
ast::VariableList *Parser<Lexer>::parse_variable_list_with_paren() {
    expected_enum_type(predicate::is_lparen, predicate::marker_lparen);
    next_token();
    auto ident_list = parse_variable_list();
    if (ident_list == nullptr) {
        return nullptr;
    }
    if (!predicate::is_rparen(current_token)) {
        delete ident_list;
        ErrorList.push_back(new PascalSParseExpectGotError(__FUNCTION__, &predicate::marker_rparen, current_token));
        return nullptr;
    }
    next_token();
    return ident_list;
}



#undef expected_keyword