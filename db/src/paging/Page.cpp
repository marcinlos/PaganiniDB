#include "config.h"
#include <paganini/paging/Page.h>
#include <cstring>
#include <string>
using std::string;

namespace paganini
{

Page::Page(page_number number, PageType type):
    header_(*(new (buffer_) PageHeader(number, type))),
    data_(buffer_ + HEADER_SIZE)
{
}

void Page::clearData()
{
    memset(data_, 0, DATA_SIZE);
}

}
