#include "config.h"
#include <paganini/paging/Page.h>
#include <cstring>

namespace paganini
{

Page::Page(page_number number, PageType type): header(number, type)
{
}

void Page::clearData()
{
    memset(data, 0, sizeof(data));
}

}
