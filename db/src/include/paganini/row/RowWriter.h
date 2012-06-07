/*
    Klasa odpowiedzialna za fizyczna serializacje wiersza. Obecnie stosowany
    jest nastepujacy format:
    
    Ile      Znaczenie
     2       Flagi wiersza   
     2       Ilosc kolumn
  ceil(N/8)  Bitmapa NULL-i
     2       Laczna dlugosci danych pol stalej wielkosci
     n       Dane stalej dlugosci
     2       Ilosc kolumn o zmiennej dlugosci
2x (zmienne+1)  Offsety pol zmiennej dlugosci + offset za koniec
     n       Pola zmiennej dlugosci     
*/
#ifndef __PAGANINI_ROW_ROW_WRITER_H__
#define __PAGANINI_ROW_ROW_WRITER_H__

#include <paganini/row/Row.h>
#include <paganini/util/bits.h>
#include <paganini/row/OutputBinaryStream.h>
#include <paganini/row/FieldFactory.h>
#include <vector>
#include <algorithm>


#include <iostream>

namespace paganini
{


class RowWriter
{
private:
    // Tworzy bitmape NULL-i w wierszu
    util::Bitmap createNullBitmap(const Row& row)
    {
        size16 cols = row.columnCount();
        util::Bitmap bmp(cols);
        
        for (int i = 0; i < cols; ++ i)
        {
            bmp[i] = row.isNull(i);
        }
        return bmp;
    }
    
    // Oblicza sumaryczny rozmiar pol stalej wielkosci.
    size16 totalFixedSize(const RowFormat& format, const FieldFactory& factory)
    {
        size16 sum = 0;
        for (const Column& col: format.fixed())
        {
            sum += factory.size(col.type);
        }
        return sum;
    }

public:
    size16 write(raw_data buffer, const Row& row)
    {
        FieldFactory& factory = FieldFactory::getInstance();
        const RowFormat& format = row.format();
        OutputBinaryStream stream(buffer);
        stream.write(row.flags());
        stream.write(row.columnCount());

        // Bitmapa nulli
        util::Bitmap null_bitmap = createNullBitmap(row);
        stream.writeRange(null_bitmap.bytes_begin(), null_bitmap.bytes_end());
        
        stream.write(totalFixedSize(format, factory));
        
        // Zapisujemy dane stalej dlugosci
        for (auto i: format.fixedIndices())
        {
            if (row[i] != nullptr)
                row[i]->writeTo(stream);
            else 
                stream.skip(factory.size(format[i].type));
        }
            
        stream.write(row.variableColumnCount());

        page_offset offset = stream.getOffset() 
            + (row.variableColumnCount() + 1) * sizeof(page_offset);        
        for (auto field: row.variable())
        {
            stream.write(offset);
            if (field != nullptr)
                offset += field->size();
        }
        // Offset za koniec
        stream.write(offset);
            
        // Zapisuje same dane
        for (auto field: row.variable())
        {
            if (field != nullptr)
                field->writeTo(stream);
        }
        return stream.getOffset();
    }
};


}


#endif // __PAGANINI_ROW_ROW_WRITER_H__
