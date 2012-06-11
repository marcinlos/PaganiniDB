/**
 *  @file StringUtils.h
 *  @brief Funkcje pomocnicze do obrobki tekstu
 */
#ifndef __PAGANINI_UTIL_LEXER_STRING_UTILS_H__
#define __PAGANINI_UTIL_LEXER_STRING_UTILS_H__

#include <string>


namespace paganini
{

namespace util
{

namespace lexer
{

/**
*  Zamienia duze litery (A-Z, bez polskich znakow) na male.
*/
inline char toLower(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - ('A' - 'a');
    else return c;
}

/**
*  Sprawdza, czy znak nalezy do podanego zbioru.
*  @param c Znak do sprawdzenia
*  @param chars Sprawdzany zbior znakow, w ktorym szukamy c
*/
inline bool isOneOf(char c, const std::string& chars)
{
    return chars.find(c) != std::string::npos;
}

/**
*  Sprawdza, czy podany znak jest litera.
*/
inline bool isAlpha(char c)
{
    return isOneOf(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

/**
*  Sprawdza, czy podany znak jest cyfra dziesietna.
*/
inline bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

/**
*  Sprawdza, czy podany znak jest niezerowa cyfra dziesietna.
*/
inline bool isNonzeroDigit(char c)
{
    return c > '0' && c <= '9';
}

/**
*  Sprawdza, czy podany znak jest cyfra osemkowa (0-7)
*/
inline bool isOctDigit(char c)
{
    return c >= '0' && c <= '7';
}

/**
*  Sprawdza, czy podany znak jest cyfra heksadecymalna.
*  Duze litery tez sa akceptowane.
*/
inline bool isHexDigit(char c)
{
    return isOneOf(c, "0123456789abcdefABCDEF");
}

/**
*  Sprawdza, czy podany znak jest alfanumeryczny.
*/
inline bool isAlphaNumeric(char c)
{
    return isOneOf(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789");
}

/**
*  Sprawdza, czy podany znak moze pojawic sie na poczatku
*  identyfikatora w stylu C++.
*/
inline bool canBeginId(char c)
{
    return isOneOf(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
}

/**
*  Sprawdza, czy podany znak moze pojawic sie wewnatrz identyfikatora 
*  w stylu C++ (niekoniecznie jako pierwszy).
*/
inline bool canBeInId(char c)
{
    return isOneOf(c, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
        "0123456789");
}

/**
*  Sprawdza, czy znak jest bialym znakiem.
*/
inline bool isWhite(char c)
{
    return isOneOf(c, " \a\b\f\n\r\t\v");
}

/**
*  Sprawdza, czy znak moze byc operatorem/elementem operatora.
*  Obecnie akceptowane znaki: . ~ ! @ # $ % ^ & * - + = | / < > ? :
*/
inline bool canBeInOperator(char c)
{
    return isOneOf(c, ",.~!@#$%^&*-+=|/<>?:");
}

/**
*  Konwertuje cyfre decymalna (char) do jej wartosci.
*/
inline int decToVal(char c)
{
    return c - '0';
}

/**
*  Konwertuje cyfre oktalna (char) do jej wartosci.
*/
inline int octToVal(char c)
{
    return c - '0';
}

/**
*  Konwertuje cyfre heksadecymalna (char) do jej wartosci.
*/
inline int hexToVal(char c)
{
    return (isDigit(c) ? c - '0' : toLower(c) - 'a' + 10);
}

   
} // lexer
} // util
} //paganini


#endif // __PAGANINI_UTIL_LEXER_STRING_UTILS_H__
