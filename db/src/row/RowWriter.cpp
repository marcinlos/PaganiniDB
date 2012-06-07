#include "config.h"
#include <paganini/row/RowWriter.h>

#include <vector>
#include <algorithm>

namespace paganini
{


util::Bitmap RowWriter::createNullBitmap(const Row& row)
{
    size16 cols = row.columnCount();
    util::Bitmap bmp(cols);
    
    for (int i = 0; i < cols; ++ i)
    {
        bmp[i] = row.isNull(i);
    }
    return bmp;
}


size16 RowWriter::totalFixedSize(const RowFormat& format, 
    const FieldFactory& factory)
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


}
