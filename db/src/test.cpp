#include "config.h"
#include "paging/Page.h"
#include "paging/PageManager.h"
#include "error_msg.h"
#include <cstdio>
using namespace paganini;


int main()
{
    printf("Size: %d\n", sizeof(Page));
    PageManager manager;
    if (manager.createFile("dupa") < 0)
        fatal_pdb("WTF??");
    manager.openFile("dupa");
    printf("No niby jest\n");
    printf("%u, %u\n", DATA_SIZE, DATA_OFFSET);
    int i;
    for (i = 0; i < 10; ++ i)
    {
        manager.allocPage();
    }
    manager.closeFile();
    return 0;
}
