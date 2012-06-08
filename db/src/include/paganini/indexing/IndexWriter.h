/*
*/
#ifndef __PAGANINI_INDEXING_INDEX_WRITER_H__
#define __PAGANINI_INDEXING_INDEX_WRITER_H__

#include <paganini/indexing/Index.h>
#include <paganini/row/datatypes.h>


namespace paganini
{


class IndexWriter
{
public:
    // Zapisuje indeks do podanego bufora
    size16 write(raw_data buffer, const Index& row);
    
    // Zwraca rozmiar fizycznej reprezentacji indeksu
    size16 size(const Index& row) const;
};


}

#endif // __PAGANINI_INDEXING_INDEX_WRITER_H__
