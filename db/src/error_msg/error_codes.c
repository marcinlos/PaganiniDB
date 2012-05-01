#include "config.h"
#include <stddef.h>

// Rozmiar tablicy
#define ERROR_COUNT 256

static int pdb_errno;

// Tablica tekstowych reprezentacji bledow
static const char* messages[ERROR_COUNT] = {
    "No error"                     // 0 (PDBE_NOERROR)
   ,"OMG blood everywhere"         // 1 (PDBE_ARMAGEDON)
   ,"Cannot create file"           // 2 (PDBE_FILECREATE)
   ,"Cannot open file"             // 3 (PDBE_FILEOPEN)
   ,"Write error"                  // 4 (PDBE_WRITE)
   ,"Read error"                   // 5 (PDBE_READ)
   ,"Positioning (lseek) error"    // 6 (PDBE_SEEK)
};


int pdbErrno()
{
    return pdb_errno;
}

int _pdbSetErrno(int errno)
{
    pdb_errno = errno;
}

const char* pdbErrorMsg(int error)
{
    if (error > 0 && error < ERROR_COUNT)
        return messages[error];
    return NULL;
}




