/*
*/
#ifndef __PAGANINI_INDEXING_BTREE_H__
#define __PAGANINI_INDEXING_BTREE_H__

#include <paganini/paging/types.h>
#include <paganini/paging/Page.h>
#include <paganini/paging/DataPage.h>


namespace paganini
{
namespace indexing
{

template <
    class PagingSystem
    class Index,
    class Reader,
    class Writer,
    class IndexReader,
    class IndexWriter
    >
class BTree
{

private:
    PagingSystem& pager_;
    page_number root_;
};



} // indexing
} // paganini


#endif // __PAGANINI_INDEXING_BTREE_H__
