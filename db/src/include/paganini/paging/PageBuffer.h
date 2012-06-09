/*
    Klasa bufora strony. Udostepnia sekcje naglowka i danych, jak rowniez 
    metody pomocnicze get() i create().
*/
#ifndef __PAGANINI_PAGING_PAGE_BUFFER_H__
#define __PAGANINI_PAGING_PAGE_BUFFER_H__
 
#include <paganini/paging/PageHeader.h>
#include <paganini/paging/configuration.h>
#include <paganini/paging/types.h>
#include <utility>


namespace paganini
{


class PageBuffer
{    
public:
    PageHeader& header;
    const raw_data data;
    char buffer[PAGE_SIZE];
    
    // Konstruktor uzupelniajacy podstawowe informacje naglowka strony
    explicit PageBuffer(page_number number = NULL_PAGE, 
        PageType type = PageType::UNUSED);
    
    // Wypelnia sekcje danych zerami
    void clearData();
    
    // Zwraca wskaznik za obszar danych strony
    inline raw_data back();
    inline const_raw_data back() const;
    
    // Pomocnicze do uzywania sekcji danych
    template <typename T>
    inline T* get();

    template <typename T>
    inline const T* get() const;
    
    // Umozliwia konstrukcje obiektu w sekcji danych
    template <typename T, typename... Args>
    inline T* create(Args&&... args);
};


// Implementacja funkcji inline

raw_data PageBuffer::back()
{
    return buffer + sizeof(buffer);
}


const_raw_data PageBuffer::back() const
{
    return buffer + sizeof(buffer);
}


template <typename T>
T* PageBuffer::get()
{
    return reinterpret_cast<T*>(data);
}


template <typename T>
const T* PageBuffer::get() const
{
    return reinterpret_cast<const T*>(data);
}


template <typename T, typename... Args>
T* PageBuffer::create(Args&&... args)
{
    return new (data) T(std::forward<Args>(args)...);
}


}

#endif // __PAGANINI_PAGING_PAGE_BUFFER_H__

