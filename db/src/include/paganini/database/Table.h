/*
*/
#ifndef __PAGANINI_DATABASE_TABLE_H__
#define __PAGANINI_DATABASE_TABLE_H__

#include <paganini/paging/types.h>
#include <paganini/row/RowFormat.h>
#include <memory>


namespace paganini
{
namespace engine
{

template <typename _DataStructure>
class Table
{
private:
    _DataStructure storage_;
    std::shared_ptr<RowFormat> format_;
    
public:
    page_number rootPage() const 
    { 
        return storage_.root(); 
    }
    
    
};


} // engine
} // paganini


#endif // __PAGANINI_DATABASE_TABLE_H__
