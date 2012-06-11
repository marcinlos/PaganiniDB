/*
*/
#ifndef __PAGANINI_UTIL_LEXER_SCRIPT_LEXER_H__
#define __PAGANINI_UTIL_LEXER_SCRIPT_LEXER_H__

#include <string>
#include <set>


namespace paganini
{

namespace util
{

namespace lexer
{

/**
 *  Typ tokena. Uzywany przez ScriptLexer, ale w przyszlosci byc moze
 *  przez inne klasy tokenizujace, choc watpie, by byly potrzebne.
 */
enum TokenType 
{ 
    T_NONE,     ///< Brak - blad
    T_INTEGER,  ///< Liczba calkowita
    T_FLOAT,    ///< Liczba zmiennoprzecinkowa
    T_CHAR,     ///< Znak, lub sekwencja ucieczki w apostrofach
    T_STRING,   ///< Napis w cudzyslowach
    T_NAME,     ///< Identyfikator
    T_OPERATOR, ///< Operator
    T_LPAR,     ///< Nawias otwierajacy: (
    T_RPAR,     ///< Nawias zamykajacy: )
    T_LBRACKET, ///< Kwadratowy nawias otwierajacy: [
    T_RBRACKET, ///< Kwadratowy nawias zamykajacy: ]
    T_LBRACE,   ///< Klamrowy nawias otwierajacy: {
    T_RBRACE    ///< Klamrowy nawias zamykajacy: }
};


/**
 *  Struktura tokena. Chcialbym, zeby tu pojawila sie tez linia
 *  i kolumna wystapienia - moze kiedys to zrobie.
 */
struct Token
{
    TokenType type; ///< Typ tokenu (TokenType)
    std::string content; ///< Tekst tokenu
    
    Token(): type(T_NONE) { }
};

/**
 *  Klasa dostarczajaca przykladowego obiektu funkcyjnego dla Tokenizer.
 *  Dokonuje tokenizacji (w przyblizeniu) takiej, jakiej poddawany jest
 *  kod C++. Operatory (jedno- i wieloznakowe) sa rejestrowane recznie.
 */
class Lexer
{
private:
    std::string operators; ///< Lista operatorow jednoznakowe
    std::set<std::string> multicharOperators; ///< Operatory wieloznakowe
   
public:
    /**
    *  Obiekt funkcyjny przekazywany do klasy Tokenizer. Oddzielony od 
    *  ScriptLexer celem zachowania prostoty interfejsu.
    */
    struct TokenizingFunction
    {
        typedef std::string::const_iterator iterator;
        Lexer& lexer;
        TokenizingFunction(Lexer& l): lexer(l) { }
        bool operator () (iterator& next, iterator end, Token& token);
        
    private:
        void skipWhite(iterator& next, iterator end);
        bool parseOperator(iterator& next, iterator end, Token& token);
        bool parseSymbol(iterator& next, Token& token);
        void removeComment(iterator& next, iterator end);
    };

    /**
    *  Tworzy obiekt funkcyjny dla Tokenizer.
    */
    TokenizingFunction function()
    {
        return TokenizingFunction(*this);
    }

    /**
    *  Rejestruje znaki jako poprawne operatory.
    *  @param ops Ciag znakow, ktore beda operatorami
    *  @return Informacja o powodzeniu wywolania
    */
    bool registerOperators(const std::string& ops);

    /**
    *  Rejestruje operator wieloznakowy.
    *  @param op Ciag znakow bedacy operatorem wieloznakowym
    *  @return Informacja o powodzeniu wywolania
    */
    bool registerMulticharOperator(const std::string& op);
};

} // lexer
} // util
} // paganini


#endif // __PAGANINI_UTIL_SCRIPT_LEXER_H__
