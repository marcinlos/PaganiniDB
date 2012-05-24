/*
    Klasa odpowiedzialna za fizyczna serializacje wiersza. Obecnie stosowany
    jest nastepujacy format:
    
    Ile      Znaczenie
     2       Flagi wiersza   
     2       Ilosc kolumn
     2       Laczna dlugosci danych pol stalej wielkosci
     n       Dane stalej dlugosci
     2       Ilosc kolumn o zmiennej dlugosci
  ceil(N/8)  Bitmapa NULL-i
 2x zmienne  Offsety pol zmiennej dlugosci
     n       Pola zmiennej dlugosci     
*/
#ifndef __PAGANINI_ROW_ROW_WRITER_H__
#define __PAGANINI_ROW_ROW_WRITER_H__

#include <paganini/row/Row.h>
#include <paganini/util/bits.h>
#include <paganini/row/OutputBinaryStream.h>
#include <vector>
#include <algorithm>


#include <iostream>

namespace paganini
{


class RowWriter
{
private:
    typedef std::vector<char> bitmap;
    
    // Tworzy bitmape NULL-i w wierszu
    bitmap createNullBitmap(const Row& row)
    {
        size16 cols = row.columnCount();
        bitmap bmp(util::min_bytes(cols));
        char* bytes = &bmp[0];
        
        for (int i = 0; i < cols; ++ i)
        {
            if (! row.isNull(i))
                util::set_bit(bytes, i);
        }
        return bmp;
    }

public:
    size16 write(raw_data buffer, const Row& row)
    {
        const RowFormat& format = row.format();
        OutputBinaryStream stream(buffer);
        stream.write(row.flags());
        stream.write(row.columnCount());
        stream.write(format.totalFixedSize());
        
        // Zapisujemy dane stalej dlugosci
        for (auto i: format.fixedIndices())
        {
            if (row[i] != nullptr)
                row[i]->writeTo(stream);
            else 
                stream.skip(format[i].size);
        }
            
        stream.write(row.variableColumnCount());
        
        // Bitmapa nulli
        bitmap null_bitmap = createNullBitmap(row);
        stream.writeRange(null_bitmap.begin(), null_bitmap.end());

        page_offset offset = stream.getOffset() 
            + row.variableColumnCount() * sizeof(page_offset);        
        for (auto field: row.variable())
        {
            stream.write(offset);
            if (field != nullptr)
                offset += field->size();
        }
            
        // Zapisuje same dane
        for (auto field: row.variable())
        {
            if (field != nullptr)
                field->writeTo(stream);
        }
    }
};


}


#endif // __PAGANINI_ROW_ROW_WRITER_H__
