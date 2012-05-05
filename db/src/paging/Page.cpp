#include "config.h"
#include <paganini/paging/Page.h>
#include <cstring>
#include <string>
using std::string;

namespace paganini
{

Page::Page(page_number number, PageType type):
    header(*(new (buffer) PageHeader(number, type))),
    data(buffer + HEADER_SIZE)
{
}

void Page::clearData()
{
    memset(data, 0, DATA_SIZE);
}

}
