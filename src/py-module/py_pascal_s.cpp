//
// Created by kamiyoru on 2020/7/1.
//

#include <pascal-s/token.h>
#include <pascal-s/llvm-ast.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <pascal-s/lexer.h>
#include <sstream>

using ListCasterBase = pybind11::detail::list_caster<std::vector<Token *>, Token *>;
namespace pybind11::detail {
    template<>
    struct type_caster<std::vector<Token *>> : ListCasterBase {
        static handle cast(const std::vector<Token *> &src, return_value_policy, handle parent) {
            return ListCasterBase::cast(src, return_value_policy::reference, parent);
        }

        static handle cast(const std::vector<Token *> *src, return_value_policy pol, handle parent) {
            return cast(*src, pol, parent);
        }
    };
}

const Marker &reinterpret_as_marker(const Token *tt) {
    return *reinterpret_cast<const Marker *>(tt);
}

const Keyword &reinterpret_as_keyword(const Token *tt) {
    return *reinterpret_cast<const Keyword *>(tt);
}


char *cs(const char *content, int length) {
    if (content == nullptr) {
        char *ns = new char[1];
        ns[0] = 0;
        return ns;
    }

    char *ns = new char[length + 1];
    strcpy_s(ns, length + 1, content);
    return ns;
}

char *cs(const char *content) {
    return cs(content, strlen(content));
}

ErrorToken *reinterpret_as_error_token(const Token *src) {
    auto dst = new ErrorToken("");

    dst->length = src->length;
    dst->offset = src->offset;
    dst->line = src->line;
    dst->column = src->column;
    dst->content = cs(reinterpret_cast<const ErrorToken *>(src)->content);
    dst->hint = cs(reinterpret_cast<const ErrorToken *>(src)->hint);
    return dst;
}

Identifier *reinterpret_as_identifier(const Token *src) {
    auto dst = new Identifier("");

    dst->length = src->length;
    dst->offset = src->offset;
    dst->line = src->line;
    dst->column = src->column;
    dst->content = cs(reinterpret_cast<const Identifier *>(src)->content);
    return dst;
}

Comment *reinterpret_as_comment(const Token *src) {
    auto dst = new Comment("");

    dst->length = src->length;
    dst->offset = src->offset;
    dst->line = src->line;
    dst->column = src->column;
    dst->content = cs(reinterpret_cast<const Comment *>(src)->content);
    return dst;
}

const ConstantInteger &reinterpret_as_constant_integer(const Token *tt) {
    return *reinterpret_cast<const ConstantInteger *>(tt);
}

ConstantReal *reinterpret_as_constant_real(const Token *tt) {
    auto dst = new ConstantReal("", 0);

    dst->length = tt->length;
    dst->offset = tt->offset;
    dst->line = tt->line;
    dst->column = tt->column;
    dst->content = cs(reinterpret_cast<const ConstantReal *>(tt)->content);
    dst->attr = (reinterpret_cast<const ConstantReal *>(tt)->attr);
    return dst;
}

const ConstantChar &reinterpret_as_constant_char(const Token *tt) {
    return *reinterpret_cast<const ConstantChar *>(tt);
}

const ConstantBoolean &reinterpret_as_constant_boolean(const Token *tt) {
    return *reinterpret_cast<const ConstantBoolean *>(tt);
}

const ConstantString &reinterpret_as_constant_string(const Token *tt) {
    return *reinterpret_cast<const ConstantString *>(tt);
}

class PascalSLexer : public FullInMemoryLexer {
public:
    PascalSLexer() : FullInMemoryLexer(nullptr, nullptr) {

    }

    const token_container &lex_once(const std::string &s) {
        this->setOption(Lexer::LexerOptionLexComment);
        std::istringstream is(s);
        this->yy_init = 0;
        this->yy_start = 0;
        this->yyrestart(&is);
        return this->get_all_tokens();
    }
};


template<typename TokenDerived>
pybind11::class_<TokenDerived> &apply_token_class_property(pybind11::class_<TokenDerived> &c) {
    return c
            .def_readwrite("type", &Token::type)
            .def_readwrite("length", &Token::length)
            .def_readwrite("offset", &Token::offset)
            .def_readwrite("column", &Token::column)
            .def_readwrite("line", &Token::line);
}

PYBIND11_MODULE(py_pascal_s, m) {
    m.doc() = "pybind11 pascal-s plugin"; // optional module docstring

    m.def("reinterpret_as_marker", &reinterpret_as_marker, "reinterpret as marker");
    m.def("reinterpret_as_error_token", &reinterpret_as_error_token, "reinterpret as error_token");
    m.def("reinterpret_as_comment", &reinterpret_as_comment, "reinterpret as comment");
    m.def("reinterpret_as_keyword", &reinterpret_as_keyword, "reinterpret as keyword");
    m.def("reinterpret_as_identifier", &reinterpret_as_identifier, "reinterpret as identifier");
    m.def("reinterpret_as_constant_integer", &reinterpret_as_constant_integer, "reinterpret as constant_integer");
    m.def("reinterpret_as_constant_real", &reinterpret_as_constant_real, "reinterpret as constant_real");
    m.def("reinterpret_as_constant_char", &reinterpret_as_constant_char, "reinterpret as constant_char");
    m.def("reinterpret_as_constant_boolean", &reinterpret_as_constant_boolean, "reinterpret as constant_boolean");
    m.def("reinterpret_as_constant_string", &reinterpret_as_constant_string, "reinterpret as constant_string");

    pybind11::enum_<TokenType>(m, "PascalSTokenType")
            .value("Unknown", TokenType::Unknown)
            .value("ConstantString", TokenType::ConstantString)
            .value("ConstantChar", TokenType::ConstantChar)
            .value("ConstantReal", TokenType::ConstantReal)
            .value("ConstantInteger", TokenType::ConstantInteger)
            .value("ConstantBoolean", TokenType::ConstantBoolean)
            .value("Identifier", TokenType::Identifier)
            .value("Nullptr", TokenType::Nullptr)
            .value("ErrorToken", TokenType::ErrorToken)
            .value("Comment", TokenType::Comment)
            .value("Length", TokenType::Length)
            .value("ConstRangeL", TokenType::ConstRangeL)
            .value("ConstRangeR", TokenType::ConstRangeR)

            .value("Range", MarkerType::Range)
            .value("LogicAnd", MarkerType::LogicAnd)
            .value("LogicOr", MarkerType::LogicOr)
            .value("LogicNot", MarkerType::LogicNot)
            .value("NEQ", MarkerType::NEQ)
            .value("LE", MarkerType::LE)
            .value("GE", MarkerType::GE)
            .value("LT", MarkerType::LT)
            .value("EQ", MarkerType::EQ)
            .value("GT", MarkerType::GT)
            .value("Add", MarkerType::Add)
            .value("Sub", MarkerType::Sub)
            .value("Mul", MarkerType::Mul)
            .value("Div", MarkerType::Div)
            .value("Mod", MarkerType::Mod)
            .value("LParen", MarkerType::LParen)
            .value("RParen", MarkerType::RParen)
            .value("LBracket", MarkerType::LBracket)
            .value("RBracket", MarkerType::RBracket)
            .value("Assign", MarkerType::Assign)
            .value("Comma", MarkerType::Comma)
            .value("Dot", MarkerType::Dot)
            .value("Semicolon", MarkerType::Semicolon)
            .value("Colon", MarkerType::Colon)
            .value("Program", KeywordType::Program)
            .value("Const", KeywordType::Const)
            .value("Var", KeywordType::Var)
            .value("Procedure", KeywordType::Procedure)
            .value("Function", KeywordType::Function)
            .value("Begin", KeywordType::Begin)
            .value("End", KeywordType::End)
            .value("Array", KeywordType::Array)
            .value("Integer", KeywordType::Integer)
            .value("Real", KeywordType::Real)
            .value("Boolean", KeywordType::Boolean)
            .value("Char", KeywordType::Char)
            .value("If", KeywordType::If)
            .value("Then", KeywordType::Then)
            .value("Else", KeywordType::Else)
            .value("For", KeywordType::For)
            .value("To", KeywordType::To)
            .value("Do", KeywordType::Do)
            .value("Of", KeywordType::Of)
            .value("Read", KeywordType::Read)
            .value("Write", KeywordType::Write)
            .export_values();

    pybind11::enum_<ast::Type>(m, "PascalSASTType")
            .export_values();

    pybind11::enum_<MarkerType>(m, "PascalSMarkerType")
            .export_values();

    auto token_class = pybind11::class_<Token>(m, "PascalSToken");
    apply_token_class_property(token_class);

    auto keyword_class = pybind11::class_<Keyword>(m, "PascalSTokenKeyword");
    apply_token_class_property(keyword_class)
            .def_readwrite("key_type", &Keyword::type);

    auto comment_class = pybind11::class_<Comment>(m, "PascalSTokenComment");
    apply_token_class_property(comment_class)
            .def_readwrite("content", &Comment::content);

    auto error_token_class = pybind11::class_<ErrorToken>(m, "PascalSTokenErrorToken");
    apply_token_class_property(error_token_class)
            .def_readwrite("content", &ErrorToken::content)
            .def_readonly("hint", &ErrorToken::hint);

    auto marker_class = pybind11::class_<Marker>(m, "PascalSTokenMarker");
    apply_token_class_property(marker_class)
            .def_readwrite("marker_type", &Marker::type);

    auto const_string_class = pybind11::class_<ConstantString>(m, "PascalSTokenConstantString");
    apply_token_class_property(const_string_class)
            .def_readwrite("attr", &ConstantString::attr);

    auto const_real_class = pybind11::class_<ConstantReal>(m, "PascalSTokenConstantReal");
    apply_token_class_property(const_real_class)
            .def_readonly("content", &ConstantReal::content)
            .def_readwrite("attr", &ConstantReal::attr);

    auto const_integer_class = pybind11::class_<ConstantInteger>(m, "PascalSTokenConstantInteger");
    apply_token_class_property(const_integer_class)
            .def_readwrite("attr", &ConstantInteger::attr);

    auto const_char_class = pybind11::class_<ConstantChar>(m, "PascalSTokenConstantChar");
    apply_token_class_property(const_char_class)
            .def_readwrite("attr", &ConstantChar::attr);

    auto const_boolean_class = pybind11::class_<ConstantBoolean>(m, "PascalSTokenConstantBoolean");
    apply_token_class_property(const_boolean_class)
            .def_readwrite("attr", &ConstantBoolean::attr);

    auto identifier_class = pybind11::class_<Identifier>(m, "PascalSTokenIdentifier");
    apply_token_class_property(identifier_class)
            .def_readwrite("content", &Identifier::content);


    pybind11::class_<PascalSLexer>(m, "PascalSLexer")
            .def(pybind11::init<>())
            .def("lex", &PascalSLexer::lex_once);

//    pybind11::class_<Node>(m, "Node")
//            .def(pybind11::init<ast::Type>())
//            .def_readonly("type", &Node::type)
//            .def_readonly("children", &Node::children)
//            .def("GetTokenSymbol", &Node::GetTokenSymbol);
}
