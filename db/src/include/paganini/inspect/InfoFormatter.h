/*
*/
#ifndef __PAGANINI_INSPECT_INFO_FORMATTER_H__
#define __PAGANINI_INSPECT_INFO_FORMATTER_H__

#include <paganini/paging/DatabaseHeader.h>
#include <paganini/paging/PageHeader.h>
#include <paganini/paging/configuration.h>
#include <paganini/paging/Page.h>
#include <paganini/paging/DataPage.h>
#include <paganini/inspect/primitives.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
using std::string;


namespace paganini
{

namespace inspect
{


class InfoFormatter
{
public:
    string operator ()(const PageHeader& header) const;
    
    string operator ()(const DatabaseHeader& header) const;
    
    string operator ()(const PageBuffer& buffer, int bytes = -1) const;

};


} // inspect
} // paganini


#endif // __PAGANINI_INSPECT_INFO_FORMATTER_H__
