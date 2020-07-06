
#include <pascal-s/interface.h>
#include <pascal-s/parser_recursive.h>

template<typename Lexer>
Parser *parser_factory(Lexer &lexer) {
    return new RecursiveParser<Lexer>(LexerProxy<Lexer>(lexer));
}

#ifdef WITH_MOCK
#include <pascal-s/lexer_mock.h>
template
struct RecursiveParser<MockLexer>;
template Parser* parser_factory(MockLexer& lexer);
#endif

#ifdef WITH_PASCAL_LEXER_FILES

#include <pascal-s/lexer.h>

//#include <pascal-s/lexer_full_in_memory.h>
template
struct RecursiveParser<FullInMemoryLexer>;

template Parser *parser_factory(FullInMemoryLexer &lexer);

#endif
