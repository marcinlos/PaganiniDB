#include "config.h"
#include "paging/page.h"
#include <stdio.h>

int main()
{
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
