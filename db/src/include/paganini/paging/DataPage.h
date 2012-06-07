/*
    Klasa reprezentujaca pojedyncza strone z szeroko pojetymi wierszami.
    Moga to byc konkretne wiersze z tabel, indeksy badz cokolwiek innego.
    
    RowType to typ przechowywanych wartosci. Writer natomiast to klasa zdolna
    zapisywac obiekty typu RowType. Udostepniac powinna nastepujace metody:
    - write(raw_data buffer, const RowType& row) - zapisuje do podanej pamieci
    - size(const RowType& row) - zwraca rozmiar wiersza po zapisaniu write-m
*/
#ifndef __PAGANINI_PAGING_DATA_PAGE_H__
#define __PAGANINI_PAGING_DATA_PAGE_H__

#include <paganini/paging/types.h>
#include <paganini/util/ReverseArray.h>
#include <paganini/paging/Page.h>
#include <paganini/row/Row.h>
#include <paganini/util/format.h>
#include <memory>
#include <stdexcept>

namespace paganini
{

template <typename RowType, typename Writer>
class DataPage
{    
public:
    // Tworzy strone danych z nowym buforem strony
    DataPage();
    
    // Tworzy strone danych z istniejacego juz buforu strony
    DataPage(std::shared_ptr<Page> page);
    
    // Konstruktor kopiujacy. Bufor strony jest wspoldzielony przez obydwa
    // obiekty.
    DataPage(const DataPage& other);
    
    // Zwraca naglowki
    inline PageHeader& header();
    inline const PageHeader& header() const;
    
    // Zwraca obiekty strony (bufor)
    inline Page& page();
    inline const Page& page() const;
    
    // Zwraca ilosc wierszy przechowywanych na stronie
    inline size16 rowCount() const;
    
    // Sprawdza, czy dany wiersz zmiesci sie na stronie.
    bool canFit(const RowType& row) const;
    
    // Zapisuje wiersz tak, aby byl widziany jako wiersz o indeksie
    // number w tej stronie.
    void insertRow(const RowType& row, row_number number);
    
    // Usuwa wiersz o podanym indeksie
    void removeRow(row_number number);
    
    // Zwraca offset wiersza o podanym numerze. Wartosci te nie musza byc
    // monotoniczne, ulozenie danych jest dowolne.
    inline page_offset offset(row_number number) const;
    
    // Zwraca adres danych wiersza o podanym numerze
    inline raw_data rowData(row_number number);
    inline const_raw_data rowData(row_number number) const;
    
private:
    std::shared_ptr<Page> page_;
    PageHeader& header_;
    raw_data data_;    
    util::ReverseArray<page_offset*> offset_array_;
    
    void insertOffset_(row_number position, page_offset offset);
};


template <typename RowType, typename Writer>
DataPage<RowType, Writer>::DataPage(): 
    page_(new Page()), 
    header_(page_->header()), 
    data_(page_->data()),
    offset_array_(reinterpret_cast<page_offset*>(page_->back()))
{
}


template <typename RowType, typename Writer>
DataPage<RowType, Writer>::DataPage(std::shared_ptr<Page> page): 
    page_(page), 
    header_(page_->header()), 
    data_(page_->data()),
    offset_array_(reinterpret_cast<page_offset*>(page_->back()))
{
}


template <typename RowType, typename Writer>
DataPage<RowType, Writer>::DataPage(const DataPage& other): 
    page_(other.page_),
    header_(page_->header()),
    data_(page_->data()),
    offset_array_(reinterpret_cast<page_offset*>(page_->back()))
{
}


template <typename RowType, typename Writer>
PageHeader& DataPage<RowType, Writer>::header()
{
    return header_;
}


template <typename RowType, typename Writer>
const PageHeader& DataPage<RowType, Writer>::header() const
{
    return header_;
}


template <typename RowType, typename Writer>
Page& DataPage<RowType, Writer>::page()
{
    return *(page_.get());
}


template <typename RowType, typename Writer>
const Page& DataPage<RowType, Writer>::page() const
{
    return *(page_.get());
}


template <typename RowType, typename Writer>
size16 DataPage<RowType, Writer>::rowCount() const
{
    return header_.rows;
}


template <typename RowType, typename Writer>
page_offset DataPage<RowType, Writer>::offset(row_number number) const
{
    if (number >= 0 && number < header_.rows)
        return offset_array_[number];
    else 
        throw std::logic_error(util::format("Row {} does not exist; {} rows "
            "are available on page", number, header_.rows)); 
}


template <typename RowType, typename Writer>
raw_data DataPage<RowType, Writer>::rowData(row_number number)
{
    return data_ + offset(number);
}


template <typename RowType, typename Writer>
const_raw_data DataPage<RowType, Writer>::rowData(row_number number) const
{
    return data_ + offset(number);
}


template <typename RowType, typename Writer>
bool DataPage<RowType, Writer>::canFit(const RowType& row) const
{
    Writer writer;
    size16 free = header_.free_space;
    // Dane wiersza + wpis do tablicy offsetow
    size16 required = writer.size(row) + sizeof(page_offset);
    return free >= required;
}


template <typename RowType, typename Writer>
void DataPage<RowType, Writer>::insertOffset_(row_number position, 
    page_offset offset)
{
    for (int i = header_.rows - 1; i >= position; -- i)
        offset_array_[i + i] = offset_array_[i];
        
    offset_array_[position] = offset;
    
    ++ header_.rows;
    header_.free_space -= sizeof(page_offset);
}


template <typename RowType, typename Writer>
void DataPage<RowType, Writer>::insertRow(const RowType& row, 
    row_number position)
{
    if (position >= 0 && position <= header_.rows)
    {
        page_offset offset = header_.free_offset;
        
        Writer writer;
        size16 written = writer.write(data_ + offset, row);
        
        // Zapisujemy offset do tablicy
        insertOffset_(position, offset);
        
        // Uaktualniamy naglowek
        header_.free_offset += written;
        header_.free_space -= written;
    }
    else 
        throw std::logic_error(util::format("Cannot insert on position {}, "
            "{} rows are available", position, header_.rows));
}


}


#endif // __PAGANINI_PAGING_DATA_PAGE_H__
