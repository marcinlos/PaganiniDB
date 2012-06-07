#include "config.h"
#include <paganini/row/RowWriter.h>
#include <paganini/util/bits.h>
#include <paganini/io/OutputBinaryStream.h>
#include <vector>
#include <algorithm>

namespace paganini
{


RowWriter::RowWriter(): factory(FieldFactory::getInstance())
{
}


util::Bitmap RowWriter::createNullBitmap(const Row& row) const
{
    size16 cols = row.columnCount();
    util::Bitmap bmp(cols);
    
    for (int i = 0; i < cols; ++ i)
    {
        bmp[i] = row.isNull(i);
    }
    return bmp;
}


size16 RowWriter::totalFixedSize(const RowFormat& format) const
{
    size16 sum = 0;
    for (const Column& col: format.fixed())
    {
        sum += factory.size(col.type);
    }
    return sum;
}


size16 RowWriter::write(raw_data buffer, const Row& row)
{
    const RowFormat& format = row.format();
    OutputBinaryStream stream(buffer);
    stream.write(row.flags());
    stream.write(row.columnCount());

    // Bitmapa nulli
    util::Bitmap null_bitmap = createNullBitmap(row);
    stream.writeRange(null_bitmap.bytes_begin(), null_bitmap.bytes_end());
    
    stream.write(totalFixedSize(format));
    
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


size16 RowWriter::size(const Row& row) const
{
    size16 length = 0;
    // Naglowek + flagi
    length += sizeof(row.flags()) + sizeof(row.columnCount());
    
    // Bitmapa nulli
    length += util::min_bytes(row.columnCount());
    
    // Calkowity rozmiar pol stalej dlugosci
    size16 fixed = totalFixedSize(row.format());
    length += sizeof(totalFixedSize(row.format()));
    
    // Pola stalej dlugosci
    length += fixed;
    
    // Ilosc pol zmiennej dlugosci
    length += sizeof(row.variableColumnCount());
    
    // Offsety pol zmiennej dlugosci
    length += (row.variableColumnCount() + 1) * sizeof(page_offset);
    
    // Pola zmiennej dlugosci
    for (auto field: row.variable())
    {
        length += field != nullptr ? field->size() : 0;
    }
    return length;
}


}
