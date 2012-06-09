#include "config.h"
#include <paganini/paging/PageBuffer.h>
#include <cstring>
#include <string>
#include <algorithm>
using std::string;

namespace paganini
{


PageBuffer::PageBuffer(page_number number, PageType type):
    header(*(new (buffer) PageHeader(number, type))),
    data(buffer + DATA_OFFSET)
{
    // clear();
}


void PageBuffer::clearData()
{
    std::fill_n(data, DATA_SIZE, 0);
}


}
