/*
    Definicje flag/makr dotyczacych stron, oraz sama klasa strony.
    Zawarta jest tu wiekszosc definicji sterujacych rozmiarami
    uzywanych struktur danych.
*/
#ifndef __PAGANINI_PAGING_PAGE_H__
#define __PAGANINI_PAGING_PAGE_H__
 
#include <paganini/paging/PageHeader.h>
#include <paganini/paging/types.h>
#include <utility>


namespace paganini
{

// Flagi

// Ilosc bitow na typ strony
static const int BITS_PER_PAGE_TYPE = 4;

// Maska typu strony
static const page_flags PAGE_TYPE_MASK = (1 << BITS_PER_PAGE_TYPE) - 1;

// Makro zwracajace typ strony z podanej flagi
inline PageType get_page_type(page_flags flags) 
{ 
    return static_cast<PageType>(flags & PAGE_TYPE_MASK); 
}

// Makro ustawiajace typ strony na podanej fladze
inline void set_page_type(page_flags& flags, PageType type)
{
    flags &= ~PAGE_TYPE_MASK;
    flags |= static_cast<page_flags>(type);
}

// Wielkosc strony jest stala
static const int PAGE_LOG_SIZE = 13;
static const int PAGE_SIZE = (1 << PAGE_LOG_SIZE);

// Offset Row Offset Array (ROA) - jest na koncu strony, indeksy ida
// od tylu (i-ty indeks => page_addr + ROA - i * sizeof(pdbRowOffset)
static const page_offset ROA_OFFSET = PAGE_SIZE - sizeof(page_offset);

// Offset poczatku danych wierszy wzgledem poczatku strony
static const page_offset DATA_OFFSET = HEADER_SIZE;

// Wielkosc sekcji danych (wiersze + ROA) na stronie
static const size16 DATA_SIZE = PAGE_SIZE - DATA_OFFSET;

// Ilosc stron obslugiwanych przez jedna strone UV
static const size32 PAGES_PER_UV = 8;


// Klasa strony - niewiele ponad bufor. Udostepnia sekcje naglowka i danych, 
// jak rowniez metody pomocnicze get() i create().
class Page
{
private:
    PageHeader& header_;
    raw_data data_;
    char buffer_[PAGE_SIZE];
    
public:
    // Konstruktor uzupelniajacy podstawowe informacje naglowka strony
    explicit Page(page_number number = NULL_PAGE, 
        PageType type = PageType::UNUSED);
    
    // Wypelnia sekcje danych zerami
    void clearData();
    
    PageHeader& header()
    {
        return header_;
    }
    
    const PageHeader& header() const
    {
        return header_;
    }
    
    raw_data data()
    {
        return data_;
    }
    
    const_raw_data data() const
    {
        return data_;
    }
    
    raw_data buffer()
    {
        return buffer_;
    }
    
    const_raw_data buffer() const
    {
        return buffer_;
    }
    
    // Pomocnicze do uzywania sekcji danych
    template <typename T>
    T* get()
    {
        return reinterpret_cast<T*>(data_);
    }

    template <typename T>
    const T* get() const
    {
        return reinterpret_cast<const T*>(data_);
    }
    
    // Umozliwia konstrukcje obiektu w sekcji danych
    template <typename T, typename... Args>
    T* create(Args&&... args)
    {
        return new (data_) T(std::forward<Args>(args)...);
    }
};



}

#endif // __PAGANINI_PAGING_PAGE_H__

