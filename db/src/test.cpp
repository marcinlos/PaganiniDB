#include "config.h"
#include "paging/Page.h"
#include "paging/PageManager.h"
#include "Error.h"
#include <cstdio>
using namespace paganini;


int main()
{
    printf("Size: %d\n", sizeof(Page));
    try
    {
        PageManager manager;
        manager.createFile("dupa");
        manager.openFile("dupa");
        printf("No niby jest\n");
        printf("%u, %u\n", DATA_SIZE, DATA_OFFSET);
        int i;
        for (i = 0; i < 10; ++ i)
        {
            manager.allocPage();
        }
        manager.closeFile();
    }
    catch (paganini::Exception& e)
    {
        printf("%s\n%s\n", e.what(), e.getCodeMessage());
    }
    return 0;
}
