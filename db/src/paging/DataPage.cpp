#include "config.h"
#include <paganini/paging/DataPage.h>
#include <paganini/row/RowWriter.h>

namespace paganini
{


DataPage::DataPage(): 
    page_(new Page()), 
    header_(page_->header()), 
    data_(page_->data()),
    offset_array_(reinterpret_cast<page_offset*>(page_->back()))
{
}


DataPage::DataPage(std::shared_ptr<Page> page): 
    page_(page), 
    header_(page_->header()), 
    data_(page_->data()),
    offset_array_(reinterpret_cast<page_offset*>(page_->back()))
{
}


DataPage::DataPage(const DataPage& other): 
    page_(other.page_),
    header_(page_->header()),
    data_(page_->data()),
    offset_array_(reinterpret_cast<page_offset*>(page_->back()))
{
}


bool DataPage::canFit(const Row& row) const
{
    RowWriter writer;
    size16 free = header_.free_space;
    // Dane wiersza + wpis do tablicy offsetow
    size16 required = writer.size(row) + sizeof(page_offset);
    return free >= required;
}


void DataPage::insertOffset_(row_number position, page_offset offset)
{
    for (int i = header_.rows - 1; i >= position; -- i)
        offset_array_[i + i] = offset_array_[i];
        
    offset_array_[position] = offset;
    
    ++ header_.rows;
    header_.free_space -= sizeof(page_offset);
}


void DataPage::insertRow(const Row& row, row_number position)
{
    page_offset offset = header_.free_offset;
    
    RowWriter writer;
    size16 written = writer.write(data_ + offset, row);
    
    // Zapisujemy offset do tablicy
    insertOffset_(position, offset);
    
    // Uaktualniamy naglowek
    header_.free_offset += written;
    header_.free_space -= written;
}


}
