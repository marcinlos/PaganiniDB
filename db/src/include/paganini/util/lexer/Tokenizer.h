#ifndef __PAGANINI_UTIL_LEXER_TOKENIZER_H__
#define __PAGANINI_UTIL_LEXER_TOKENIZER_H__

#include <string>
#include <vector>

namespace paganini
{

namespace util
{

namespace lexer
{


/**
 *  Moja stara klasa napisana na wzor tokenizera z boost'a. Sama w sobie
 *  robi niewiele, glowna funkcjonalnosc jest w Lexer.
 *  Jest on przykladowa implementacja obiektu funkcyjnego TokenizerFunction 
 *  (patrz argumenty szablonu), ktory odpowiada za wydzielanie tokenow.
 */
template <
    typename TokenizerFunction, 
    typename TokenInfo = std::string,
    typename Iterator = std::string::const_iterator
>
class Tokenizer
{
public:
    typedef typename std::vector<TokenInfo>::iterator iterator;

    /**
    *  Zwraca iterator wskazujacy poczatek listy tokenow
    */
    iterator begin() { return tokens.begin(); }

    /**
    *  Zwraca iterator wskazujacy koniec listy tokenow
    */
    iterator end() { return tokens.end(); }

    /**
    *  Przygotowuje do tokenizowania zawartosc calego kontenera. 
    *  Musi udostepniac on metody begin() i end() zwracajace iteratory 
    *  do poczatku i konca, tak jak np. kontenery w STL-u
    *  @param container Kontener przechowywujacy znaki do stokenizowania
    *  @param tokenizerFunction Obiekt funkcyjny dokonujacy tokenizacji
    */
    template <typename Container>
    Tokenizer(const Container& container, 
        const TokenizerFunction& tokenizerFunction = TokenizerFunction());

    /**
    *  Przygotowuje do tokenizowania zakres [begin, end)
    *  @param begin Iterator poczatkowy
    *  @param end Iterator konca
    *  @param tokenizerFunction Obiekt funkcyjny dokonujacy tokenizacji
    */
    Tokenizer(Iterator begin, Iterator end, 
        const TokenizerFunction& tokenizerFunction = TokenizerFunction());

    /**
    *  Dokonuje wlasciwej tokenizacji
    */
    void tokenize();

private:
    Iterator beginOfSequence, endOfSequence;
    TokenizerFunction tokenizer;
    Iterator next;
    std::vector<TokenInfo> tokens;
};

template <
    typename TokenizerFunction, 
    typename TokenInfo,
    typename Iterator
>
template <typename Container>
Tokenizer<TokenizerFunction, TokenInfo, Iterator>::Tokenizer(
    const Container& container, const TokenizerFunction& tokenizerFunction): 
        beginOfSequence(container.begin()),
        endOfSequence(container.end()), 
        tokenizer(tokenizerFunction)
{
}

template <
    typename TokenizerFunction, 
    typename TokenInfo,
    typename Iterator
>
Tokenizer<TokenizerFunction, TokenInfo, Iterator>::Tokenizer(Iterator begin, 
    Iterator end, const TokenizerFunction& tokenizerFunction):
        beginOfSequence(begin), 
        endOfSequence(end), 
        tokenizer(tokenizerFunction)
{
}


template <
    typename TokenizerFunction, 
    typename TokenInfo,
    typename Iterator
>
void Tokenizer<TokenizerFunction, TokenInfo, Iterator>::tokenize()
{
    next = beginOfSequence;
    while (next != endOfSequence)
    {
        TokenInfo tmp;
        if (tokenizer(next, endOfSequence, tmp))
            tokens.push_back(tmp);
    }
}


} // lexer
} // util
} // paganini


#endif // __PAGANINI_UTIL_LEXER_TOKENIZER_H__
