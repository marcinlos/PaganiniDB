/*
*/
#ifndef __PAGANINI_PAGING_DATA_PAGE_H__
#define __PAGANINI_PAGING_DATA_PAGE_H__

#include <paganini/paging/types.h>
#include <paganini/paging/Page.h>
#include <paganini/row/Row.h>
#include <memory>

namespace paganini
{

class DataPage
{
private:
    std::unique_ptr<Page> page_;
    PageHeader& header_;
    raw_data data_;
    
public:
    DataPage(): 
        page_(new Page()), 
        header_(page_->header()), 
        data_(page_->data())
    {
    }
    
    DataPage(DataPage&& other): 
        page_(std::move(other.page_)),
        header_(page_->header()),
        data_(page_->data())
    {
    }
    
    // Zwraca naglowki
    PageHeader& header()
    {
        return page_->header();
    }
    
    const PageHeader& header() const
    {
        return page_->header();
    }
    
    // Zwraca obiekty strony (bufor)
    Page& page()
    {
        return *(page_.get());
    }
    
    const Page& page() const
    {
        return *(page_.get());
    }
    
    //
    
    
};


}


#endif // __PAGANINI_PAGING_DATA_PAGE_H__
