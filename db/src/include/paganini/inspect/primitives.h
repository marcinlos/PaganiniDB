/*
*/
#ifndef __PAGANINI_INSPECT_PRIMITIVES_H__
#define __PAGANINI_INSPECT_PRIMITIVES_H__

#include <paganini/paging/configuration.h>
#include <paganini/row/datatypes.h>
#include <paganini/paging/PageHeader.h>
#include <paganini/util/format.h>
#include <iostream>
#include <sstream>


namespace paganini
{

namespace inspect
{


inline string page_number_str(page_number page)
{
    if (page == NULL_PAGE)
        return "NULL_PAGE";
    else return util::format("{}", page);
}


inline string object_id_str(object_id id)
{
    if (id == NULL_OBJ)
        return "NULL_OBJ";
    else return util::format("{}", id);
}


inline string page_flags_str(page_flags flags)
{
    std::ostringstream os;
    bool first = true;
    
    if (flags & page_flags(PageFlags::LEAF))
        os << (first ? "" : " | ") << PageFlags::LEAF;
        
    if (first)
        os << "None";
        
    return os.str();
}


} // inspect
} // paganini


#endif // __PAGANINI_INSPECT_PRIMITIVES_H__
