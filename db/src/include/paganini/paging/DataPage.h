/*
    Klasa reprezentujaca pojedyncza strone z szeroko pojetymi wierszami.
    Moga to byc konkretne wiersze z tabel, indeksy badz cokolwiek innego.
    
    RowType to typ przechowywanych wartosci.
    
    FormatInfo to dodatkowe informacje przekazywane w celu umozliwienia
    readerowi odczytania zapisanych przez writera informacji. Reader/writer
    dzialaja ogolnie w oparciu np. o definicje tabeli, stad konieczne jest
    dostarczenie tego typu informacji.
    
    Writer to klasa zdolna zapisywac obiekty typu RowType. 
    Udostepniac powinna nastepujace metody:
    - write(raw_data buffer, const RowType& row) - zapisuje do podanej pamieci
    - size(const RowType& row) - zwraca rozmiar wiersza po zapisaniu write-m
    
    Reader to klasa zdolna odczytac obiekty typu RowType. Udostepnia
    nastepujace metode
        Reader::ReturnType read(raw_data buffer, const RowFormat& format)   
*/
#ifndef __PAGANINI_PAGING_DATA_PAGE_H__
#define __PAGANINI_PAGING_DATA_PAGE_H__

#include <paganini/paging/types.h>
#include <paganini/paging/Page.h>
#include <paganini/row/Row.h>
#include <paganini/util/format.h>
#include <memory>
#include <iterator>
#include <algorithm>
#include <stdexcept>


#include <iostream>

namespace paganini
{

template <
    typename RowType, 
    typename FormatInfo,
    class Reader, 
    class Writer
    >
class DataPage: public Page
{    
public:
    // Flaga oznaczajaca, czy strona jest lisciem
    static const int LEAF_FLAG = 1 << 5;
    
    // Tworzy strone danych z nowym buforem strony
    DataPage();
    
    // Tworzy strone danych z istniejacego juz buforu strony
    DataPage(std::shared_ptr<PageBuffer> page);
    
    // Konstruktor kopiujacy. Bufor strony jest wspoldzielony przez obydwa
    // obiekty.
    DataPage(const DataPage& other);
    
    // Zwraca ilosc wierszy przechowywanych na stronie
    inline size16 rowCount() const;
    
    // Sprawdza, czy dany wiersz zmiesci sie na stronie.
    bool canFit(const RowType& row) const;
    
    // Zapisuje wiersz tak, aby byl widziany jako wiersz o indeksie
    // number w tej stronie.
    void insert(const RowType& row, row_number number);
    
    // Usuwa wiersz o podanym indeksie
    void erase(row_number number);
    
    // Zwraca offset wiersza o podanym numerze. Wartosci te nie musza byc
    // monotoniczne, ulozenie danych jest dowolne.
    inline page_offset offset(row_number number) const;
    
    // Zwraca adres danych wiersza o podanym numerze
    inline raw_data rowData(row_number number);
    inline const_raw_data rowData(row_number number) const;
    
    // Zwraca obiekt reprezetujacy wiersz o podanym numerze
    typename Reader::ReturnType row(row_number number, 
        const FormatInfo& format) const;
    
private:   
    std::reverse_iterator<page_offset*> offset_array_;
    
    void insertOffset_(row_number position, page_offset offset);
};


template <typename RowType, typename FormatInfo, class Reader, class Writer>
DataPage<RowType, FormatInfo, Reader, Writer>::DataPage(): 
    offset_array_(reinterpret_cast<page_offset*>(buffer_->back()))
{
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
DataPage<RowType, FormatInfo, Reader, 
    Writer>::DataPage(std::shared_ptr<PageBuffer> page): Page(page),
    offset_array_(reinterpret_cast<page_offset*>(buffer_->back()))
{
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
DataPage<RowType, FormatInfo, Reader, Writer>::DataPage(const DataPage& other): 
    Page(other),
    offset_array_(reinterpret_cast<page_offset*>(buffer_->back()))
{
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
size16 DataPage<RowType, FormatInfo, Reader, Writer>::rowCount() const
{
    return buffer_->header.rows;
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
page_offset DataPage<RowType, FormatInfo, Reader, 
    Writer>::offset(row_number number) const
{
    if (number >= 0 && number < buffer_->header.rows)
        return offset_array_[number];
    else 
        throw std::logic_error(util::format("Row {} does not exist; {} rows "
            "are available on page", number, buffer_->header.rows)); 
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
raw_data DataPage<RowType, FormatInfo, Reader, 
    Writer>::rowData(row_number number)
{
    if (number >= 0 && number < buffer_->header.rows)
        return buffer_->data + offset(number);
    else 
        throw std::logic_error(util::format("Row {} does not exist; {} rows "
            "are available on page", number, buffer_->header.rows)); 
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
const_raw_data DataPage<RowType, FormatInfo, Reader, 
    Writer>::rowData(row_number number) const
{
    if (number >= 0 && number < buffer_->header.rows)
        return buffer_->data + offset(number);
    else 
        throw std::logic_error(util::format("Row {} does not exist; {} rows "
            "are available on page", number, buffer_->header.rows)); 
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
typename Reader::ReturnType DataPage<RowType, FormatInfo, Reader,
    Writer>::row(row_number number, const FormatInfo& format) const
{
    if (number >= 0 && number < buffer_->header.rows)
    {
        raw_data row_data = buffer_->data + offset(number);
        Reader reader;
        return reader.read(row_data, format);
    }
    else 
        throw std::logic_error(util::format("Row {} does not exist; {} rows "
            "are available on page", number, buffer_->header.rows)); 
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
bool DataPage<RowType, FormatInfo, Reader, 
    Writer>::canFit(const RowType& row) const
{
    Writer writer;
    size16 free = buffer_->header.free_space;
    // Dane wiersza + wpis do tablicy offsetow
    size16 required = writer.size(row) + sizeof(page_offset);
    return free >= required;
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
void DataPage<RowType, FormatInfo, Reader, 
    Writer>::insertOffset_(row_number position, page_offset offset)
{
    for (int i = buffer_->header.rows - 1; i >= position; -- i)
    {
        offset_array_[i + 1] = offset_array_[i];
    }
        
    offset_array_[position] = offset;
    
    ++ buffer_->header.rows;
    buffer_->header.free_space -= sizeof(page_offset);
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
void DataPage<RowType, FormatInfo, Reader, 
    Writer>::insert(const RowType& row, row_number position)
{
    if (position >= 0 && position <= buffer_->header.rows)
    {
        page_offset offset = buffer_->header.free_offset;
        
        Writer writer;
        size16 written = writer.write(buffer_->data + offset, row);
        
        // Zapisujemy offset do tablicy
        insertOffset_(position, offset);
        
        // Uaktualniamy naglowek
        buffer_->header.free_offset += written;
        buffer_->header.free_space -= written;
    }
    else 
        throw std::logic_error(util::format("Cannot insert on position {}, "
            "{} rows are available", position, buffer_->header.rows));
}


template <typename RowType, typename FormatInfo, class Reader, class Writer>
void DataPage<RowType, FormatInfo, Reader, 
    Writer>::erase(row_number position)
{
    if (position >= 0 && position <= buffer_->header.rows)
    {
        page_offset offset = offset_array_[position];
        
        // Szukamy poczatku nastepnej porcji danych
        page_offset end = buffer_->header.free_offset;
        for (int i = 0; i < buffer_->header.rows; ++ i)
        {
            page_offset n = offset_array_[i];
            if (n > offset && n < end)
                end = n;
        }
        size16 size = end - offset;

        // Przepisujemy wszystkie dane znad tych usuwanych
        raw_data data = buffer_->data;
        std::copy(data + end, data + buffer_->header.free_offset, 
            data + offset);
        
        // Uaktualniamy tablice offsetow
        for (int i = 0; i < buffer_->header.rows; ++ i)
        {
            if (offset_array_[i] > offset)
                offset_array_[i] -= size;
        }
        // Usuwamy z niej wpis o wlasnie usunietym wierszu
        for (int i = position; i < buffer_->header.rows - 1; ++ i)
            offset_array_[i] = offset_array_[i + 1];
        
        // Uaktualniamy naglowek
        buffer_->header.free_offset -= size;
        buffer_->header.free_space += size;
        -- buffer_->header.rows;
    }
    else 
        throw std::logic_error(util::format("Cannot remove row {}, "
            "{} rows are available", position, buffer_->header.rows));
}


}


#endif // __PAGANINI_PAGING_DATA_PAGE_H__
