/*
*/
#ifndef __PAGANINI_INSPECT_FORMAT_BYTES_H__
#define __PAGANINI_INSPECT_FORMAT_BYTES_H__
#include <paganini/paging/types.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
using std::string;


namespace paganini
{

namespace inspect
{

namespace 
{

inline char hex_digit(int i)
{
    return i < 10 ? i + '0' : i - 10 + 'A';
}


inline unsigned char make_ascii(unsigned char c)
{
    return c == '\0' ? '.' : (c < ' ' ? ' ' : c);
}


inline int compute_width(size16 max)
{
    int i = 1;
    do
    {
        max /= 10;
        ++ i;
    }
    while (max > 0);
    return i;
}

}


inline string format_bytes(const_raw_data data, size16 len, size16 in_line = 16)
{
    std::ostringstream ss;
    int lines = (len + in_line - 1) / in_line;
    int width = compute_width(lines);
    for (int i = 0; i < lines; ++ i)
    {
        typedef const unsigned char* bytes;
        ss << std::setw(width) << std::hex << i * in_line << " |";
        bytes d = reinterpret_cast<bytes>(data + i * in_line);
        for (int j = 0; j < in_line; ++ j)
        {
            if (j + i * in_line < len)
            {
                ss << hex_digit((d[j] & 0xf0) >> 4);
                ss << hex_digit(d[j] & 0x0f);
            }
            else
                ss << "  ";
            if (j != in_line - 1)
            {
                ss << ' '; 
                if ((j + 1) % 2 == 0)
                    ss << ' ';
            }
        }
        ss << " | ";
        for (int j = 0; j < in_line; ++ j)
        {
            if (j + i * in_line < len)
                ss << make_ascii(d[j]);
            else ss << ' ';
        }
        ss << std::endl;
    }
    return ss.str();
}


} // inspect
} // paganini

#endif // __PAGANINI_INSPECT_FORMAT_BYTES_H__

