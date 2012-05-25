/*
    Klasa odpowiedzialna za deserializacje wiersza. Dokladny opis w 
    RowWriter.h.    
*/
#ifndef __PAGANINI_ROW_ROW_READER_H__
#define __PAGANINI_ROW_ROW_READER_H__

#include <paganini/row/Row.h>
#include <paganini/util/bits.h>
#include <paganini/row/InputBinaryStream.h>
#include <paganini/row/FieldFactory.h>
#include <vector>
#include <algorithm>


#include <iostream>

namespace paganini
{


class RowReader
{
private:
    typedef std::vector<char> bitmap;
    typedef std::unique_ptr<types::Data> DataPtr;
    FieldFactory& factory;
    
    void readField(InputBinaryStream& stream, Row* row, const Column& column,
        const util::Bitmap& null_bitmap, size16 size = 0)
    {
        column_number col = column.col;
        std::unique_ptr<types::Data> data = nullptr;
        if (! null_bitmap[col])
        {
            data = factory.create(column.type);
            data->readFrom(stream, size);
        }
        else
            stream.skip(factory.size(column.type));
        (*row)[col] = data.release();
    }

public:
    RowReader(): factory(FieldFactory::getInstance())
    {
    }
    
    std::unique_ptr<Row> read(raw_data buffer, const RowFormat& format)
    {
        std::vector<DataPtr> fields(format.columnCount());
        
        InputBinaryStream stream(buffer);
        row_flags flags;
        stream.read(&flags);
        
        size16 cols;
        stream.read(&cols);
        
        util::Bitmap null_bitmap(cols);
        stream.readRange(null_bitmap.bytes_begin(), util::min_bytes(cols));
        
        size16 totalFixedSize;
        stream.read(&totalFixedSize);
        
        std::unique_ptr<Row> row(new Row(format, 0));
        
        for (const Column& column: format.fixed())
            readField(stream, row.get(), column, null_bitmap);

        size16 var_sized;
        stream.read(&var_sized);

        page_offset prev;
        stream.read(&prev);
        
        for (const Column& column: format.variable())
        {
            // Pobieramy kolejny offset, zeby obliczyc z niego rozmiar
            page_offset offset;
            stream.read(&offset);
            size16 size = offset - prev;  

            // Przechodzimy do jego danych
            page_offset current = stream.getOffset();
            stream.setOffset(prev);
            readField(stream, row.get(), column, null_bitmap, size);
            prev = offset;
            // Wracamy
            stream.setOffset(current);
        }        
        return row;
    }
};


}


#endif // __PAGANINI_ROW_ROW_READER_H__
