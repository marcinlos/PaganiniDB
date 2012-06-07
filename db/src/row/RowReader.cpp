#include "config.h"
#include <paganini/util/bits.h>
#include <paganini/io/InputBinaryStream.h>
#include <paganini/row/RowReader.h>


namespace paganini
{

RowReader::RowReader(): factory(FieldFactory::getInstance())
{
}


void RowReader::readField(InputBinaryStream& stream, Row* row, 
    const Column& column, const util::Bitmap& null_bitmap, size16 size)
{
    column_number col = column.col;
    std::unique_ptr<types::Data> data = nullptr;
    bool is = null_bitmap[col];
    if (! null_bitmap[col])
    {
        data = factory.create(column.type);
        data->readFrom(stream, size);
    }
    else
    {
        // NULL jako wartosc pola zmiennej dlugosci nie generuje zadnego
        // przesuniecia przy zapisie.
        page_offset to_skip = factory.size(column.type);
        if (to_skip != types::VARIABLE_SIZE)
            stream.skip(factory.size(column.type));
    }
    (*row)[col] = data.release();
}


void RowReader::readVariables(InputBinaryStream& stream, Row* row, 
    const util::Bitmap& null_bitmap)
{
    page_offset prev;
    stream.read(&prev);
    
    for (const Column& column: row->format().variable())
    {
        // Pobieramy kolejny offset, zeby obliczyc z niego rozmiar
        page_offset offset;
        stream.read(&offset);
        
        // Przechodzimy do jego danych
        page_offset current = stream.getOffset();
        stream.setOffset(prev);
        readField(stream, row, column, null_bitmap, offset - prev);
        prev = offset;
        
        // Wracamy do tablicy przesuniec
        stream.setOffset(current);
    }  
}


std::unique_ptr<Row> RowReader::read(raw_data buffer, const RowFormat& format)
{
    typedef std::unique_ptr<types::Data> DataPtr;
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
    
    std::unique_ptr<Row> row(new Row(format, flags));
    
    for (const Column& column: format.fixed())
        readField(stream, row.get(), column, null_bitmap);

    size16 var_sized;
    stream.read(&var_sized);

    readVariables(stream, row.get(), null_bitmap);  
    return row;
}


}
