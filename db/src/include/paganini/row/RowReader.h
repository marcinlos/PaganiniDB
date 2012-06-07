/*
    Klasa odpowiedzialna za deserializacje wiersza. Dokladny opis w 
    RowWriter.h.    
*/
#ifndef __PAGANINI_ROW_ROW_READER_H__
#define __PAGANINI_ROW_ROW_READER_H__

#include <paganini/row/Row.h>
#include <paganini/row/FieldFactory.h>


namespace paganini
{


class RowReader
{
public:
    RowReader();
    
    // Z podanego obszaru pamieci tworzy nowy wiersz w oparciu o podany
    // format i zwraca wskaznik na niego.
    std::unique_ptr<Row> read(raw_data buffer, const RowFormat& format);
    
private:
    FieldFactory& factory;
    
    // Funkcja pomocnicza wczytujaca pojedyncze pole (jako wartosc podanej
    // kolumny)
    void readField(InputBinaryStream& stream, Row* row, const Column& column,
        const util::Bitmap& null_bitmap, size16 size = 0);
    
    // Wczytuje wszystkie pola zmiennej wielkosci
    void readVariables(InputBinaryStream& stream, Row* row, 
        const util::Bitmap& null_bitmap);
};


}


#endif // __PAGANINI_ROW_ROW_READER_H__
