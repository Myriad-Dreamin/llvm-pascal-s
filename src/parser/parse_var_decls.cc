//
// Created by kamiyoru on 2020/6/27.
//


template<typename Lexer>
ast::VarDecls *RecursiveParser<Lexer>::parse_var_decls() {

    //var
    expected_enum_type(predicate::is_var, predicate::keyword_var);
    auto var_tok = current_token;
    next_token();

    // look ahead
    expected_type_r(TokenType::Identifier, nullptr);
    auto decls = _parse_var_decls(new ast::VarDecls);
    assert(decls != nullptr);
    if (decls->decls.empty()) {
        ast::copy_pos_with_check(decls, var_tok);
    } else {
        ast::copy_pos_between_tokens(decls, var_tok, decls->decls.back());
    }
    return decls;
}

template<typename Lexer>
ast::VarDecls *RecursiveParser<Lexer>::_parse_var_decls(ast::VarDecls *decls) {

    for (;;) {
        // declaration
        auto decl = parse_var_decl();
        if (decl == nullptr) {
            return decls;
        }
        decls->decls.push_back(decl);

        // look ahead
        for (;;) {
            if (current_token == nullptr) {
                return decls;
            }
            if (current_token->type == TokenType::Identifier) {
                break;
            }
            maybe_recover_keyword(KeywordType::Procedure)
            maybe_recover_keyword(KeywordType::Function)
            maybe_recover_keyword(KeywordType::Begin)

            skip_error_token_s("expect new var decl or other token");
            return fall_expect_s("expect new var decl or other token"), decls;
        }
        if (current_token == nullptr || current_token->type != TokenType::Identifier) {
            return decls;
        }
    }
}

template<typename Lexer>
ast::VarDecl *RecursiveParser<Lexer>::parse_var_decl() {

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
