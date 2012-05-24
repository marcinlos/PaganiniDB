/*
    Klasa odpowiedzialna za deserializacje wiersza. Dokladny opis w 
    RowWriter.h.    
*/
#ifndef __PAGANINI_ROW_ROW_READER_H__
#define __PAGANINI_ROW_ROW_READER_H__

#include <paganini/row/Row.h>
#include <paganini/util/bits.h>
#include <paganini/row/InputBinaryStream.h>
#include <vector>
#include <algorithm>


#include <iostream>

namespace paganini
{


class RowReader
{
private:
    typedef std::vector<char> bitmap;

public:
    std::unique_ptr<Row> read(raw_data buffer, const RowFormat& format)
    {
        InputBinaryStream stream(buffer);
        row_flags flags;
        stream.read(&flags);
        size16 cols;
        stream.read(&cols);
        // return std::unique_ptr<Row>(new Row(format, {nullptr}, 0));
    }
};


}


#endif // __PAGANINI_ROW_ROW_READER_H__
