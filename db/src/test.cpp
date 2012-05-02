#include "config.h"
#include "paging/Page.h"
#include "paging/page_manager.h"
#include "error_msg.h"
#include <cstdio>
using namespace paganini;


int main()
{
    printf("Size: %d\n", sizeof(Page));
    if (pdbCreateDatabaseFile("dupa") < 0)
        fatal_pdb("WTF??");
    pdbPageManagerStart("dupa");
    printf("No niby jest\n");
    printf("%u, %u\n", DATA_SIZE, DATA_OFFSET);
    int i;
    for (i = 0; i < 10; ++ i)
    {
        pdbAllocPage();
    }
    pdbPageManagerStop();
    return 0;
}
