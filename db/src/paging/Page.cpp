#include "config.h"
#include "paging/Page.h"

namespace paganini
{

Page::Page(page_number number, PageType type): header(number, type)
{
}

}
