/*
    Funkcja pomocnicza do formatowania stringow. Dzieki wspanialosci
    C++11 wreszcie da sie to po ludzku zrobic w tym jezyku :P
*/
#ifndef __PAGANINI_UTIL_FORMAT_H__
#define __PAGANINI_UTIL_FORMAT_H__

#include <sstream>
#include <string>
#include <utility>
using std::string;

namespace paganini
{

namespace util
{

// Ciag znakow oznaczajacy miejsce na argument
const string PLACEHOLDER = "{}";


struct _Notype {};

    
// Funkcja pomocnicza - sluzy jako typowe, perfidne domkniecie
template <typename T>
inline _Notype _aux_use_next(std::ostringstream& s, const string& format,
    string::size_type& pos, const T& val)
{
    auto next = format.find(PLACEHOLDER, pos);
    if (next == string::npos)
    {
        next = format.size();
    }
    s.write(format.data() + pos, next - pos);
    pos = next + (next == format.size() ? 0 : PLACEHOLDER.size());
    s << val;

    return {};
}


// Funkcja wlasciwa
template <typename... Args>
inline string format(const string& format, const Args&... args)
{
    std::ostringstream s;
    string::size_type pos;
    
    // Iterujemy po argumentach - nie bardzo jest jak latwiej
    std::initializer_list<_Notype>{ _aux_use_next(s, format, pos, args)... };
    
    // Dopisujemy koncowke
    if (! format.empty())
        s.write(format.data() + pos, format.size() - pos);
    return s.str();
}

} // util
} // paganini

#endif // __PAGANINI_UTIL_FORMAT_H__


