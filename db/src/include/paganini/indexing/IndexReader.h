/*
*/
#ifndef __PAGANINI_INDEXING_INDEX_READER_H__
#define __PAGANINI_INDEXING_INDEX_READER_H__

#include <paganini/indexing/Index.h>
#include <paganini/row/datatypes.h>
#include <paganini/row/FieldFactory.h>


namespace paganini
{


class IndexReader
{
public:
    typedef std::unique_ptr<Index> ReturnType;
    
    IndexReader();
    
    // Wczytuje z pamieci strukture pojedynczego wpisu indeksowego
    ReturnType read(raw_data buffer, types::FieldType format);
    
private:
    FieldFactory& factory;
};


}



#endif // __PAGANINI_INDEXING_INDEX_READER_H__
