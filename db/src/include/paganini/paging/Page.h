/*
    Klasa strony, zawierajaca podstawowe manipulacje niezalezne od jej
    rodzaju.
*/
#ifndef __PAGANINI_PAGING_PAGE_H__
#define __PAGANINI_PAGING_PAGE_H__
 
#include <paganini/paging/PageBuffer.h>
#include <paganini/paging/types.h>
#include <paganini/paging/configuration.h>
#include <memory>


namespace paganini
{


class Page
{
public:
    // Tworzy strone danych z nowym buforem strony
    Page();
    
    // Tworzy strone danych z istniejacego juz buforu strony
    explicit Page(std::shared_ptr<PageBuffer> page);
    
    // Konstruktor kopiujacy. Bufor strony jest wspoldzielony przez obydwa
    // obiekty.
    Page(const Page& other);
    
    // Setter/getter numeru strony
    inline page_number number() const;
    inline void setNumber(page_number number);
    
    // Setter/getter wlasciciela
    inline object_id owner() const;
    inline void setOwner(object_id owner);
    
    // Setter/getter typu
    inline PageType type() const;
    inline void setType(PageType type);
    
    // Setter/getter flag
    inline page_flags flags() const;
    inline void setFlags(page_flags flags);
    
    // Setter/getter nastepnika
    inline page_number next() const;
    inline void setNext(page_number next);
    
    // Setter/getter poprzednika
    inline page_number prev() const;
    inline void setPrev(page_number prev);
    
    // Zwraca bufor strony
    inline const PageBuffer* buffer() const;
    inline PageBuffer* buffer();
    
    // Zwraca naglowek
    inline const PageHeader& header() const;

protected:
    std::shared_ptr<PageBuffer> buffer_;
};



page_number Page::number() const
{
    return buffer_->header.number;
}


void Page::setNumber(page_number number)
{
    buffer_->header.number = number;
}


object_id Page::owner() const
{
    return buffer_->header.owner;
}


void Page::setOwner(object_id owner)
{
    buffer_->header.owner = owner;
}


PageType Page::type() const
{
    return get_page_type(buffer_->header.flags);
}


void Page::setType(PageType type)
{
    set_page_type(buffer_->header.flags, type);
}


page_flags Page::flags() const
{
    return buffer_->header.flags;
}


void Page::setFlags(page_flags flags)
{
    buffer_->header.flags = flags;
}


page_number Page::next() const
{
    return buffer_->header.next;
}


void Page::setNext(page_number next)
{
    buffer_->header.next = next;
}


page_number Page::prev() const
{
    return buffer_->header.prev;
}


void Page::setPrev(page_number prev)
{
    buffer_->header.prev = prev;
}


PageBuffer* Page::buffer()
{
    return buffer_.get();
}


const PageBuffer* Page::buffer() const
{
    return buffer_.get();
}


const PageHeader& Page::header() const
{
    return buffer_->header;
}


}

#endif // __PAGANINI_PAGING_PAGE_H__

