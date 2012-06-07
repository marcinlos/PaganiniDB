/*
*/
#ifndef __PAGANINI_PAGING_DATA_PAGE_H__
#define __PAGANINI_PAGING_DATA_PAGE_H__

#include <paganini/paging/types.h>
#include <paganini/util/ReverseArray.h>
#include <paganini/paging/Page.h>
#include <paganini/row/Row.h>
#include <memory>

namespace paganini
{


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
    bool canFit(const Row& row) const;
    
    // Zapisuje wiersz tak, aby byl widziany jako wiersz o indeksie
    // number w tej stronie.
    void insertRow(const Row& row, row_number number);
    
    // Usuwa wiersz o podanym indeksie
    
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


// Implementacje funkcji inline

PageHeader& DataPage::header()
{
    return header_;
}

const PageHeader& DataPage::header() const
{
    return header_;
}

Page& DataPage::page()
{
    return *(page_.get());
}

const Page& DataPage::page() const
{
    return *(page_.get());
}

size16 DataPage::rowCount() const
{
    return header_.rows;
}

page_offset DataPage::offset(row_number number) const
{
    return offset_array_[number];
}

raw_data DataPage::rowData(row_number number)
{
    return data_ + offset(number);
}

const_raw_data DataPage::rowData(row_number number) const
{
    return data_ + offset(number);
}


}


#endif // __PAGANINI_PAGING_DATA_PAGE_H__
