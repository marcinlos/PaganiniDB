#include "config.h"
#include <paganini/paging/Page.h>
#include <cstring>
#include <string>
#include <algorithm>
using std::string;

namespace paganini
{


Page::Page(): buffer_(new PageBuffer)
{
}


Page::Page(std::shared_ptr<PageBuffer> page): buffer_(page)
{
}


Page::Page(const Page& other): buffer_(other.buffer_)
{
}


}
