#include "config.h"
#include <paganini/paging/Page.h>
#include <cstring>
#include <string>
#include <algorithm>
using std::string;

namespace paganini
{


Page::Page(page_number number, PageType type):
    header_(*(new (buffer_) PageHeader(number, type))),
    data_(buffer_ + HEADER_SIZE)
{
    // clearData();
}


void Page::clearData()
{
    std::fill_n(data_, DATA_SIZE, 0);
}


}
