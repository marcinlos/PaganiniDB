/*
*/
#ifndef __PAGANINI_INDEXING_BTREE_H__
#define __PAGANINI_INDEXING_BTREE_H__

#include <paganini/paging/types.h>
#include <paganini/paging/Page.h>
#include <paganini/paging/DataPage.h>
#include <memory>


namespace paganini
{
namespace indexing
{

template <
    class PagingSystem
    class Indexer,
    class Page,
    class Row,
    class Reader,
    class Writer,
    class IndexReader,
    class IndexWriter
    >
class BTree
{
public:
    static const page_number ALLOC_NEW = -1;
    
    // Tworzy nowe B+ drzewo oparte na podanym systemie stronnicowania.
    // Jego korzen jest zapisywany do strony o numerze root. Jesli
    // root == ALLOC_NEW, strona jest pobierana poprzez metode allocPage
    // systemu stronnicowania.
    explicit BTree(PagingSystem& pager, page_number root = ALLOC_NEW);
    
    // Dodaje wiersz do drzewa
    bool insert(const Row& row);

private:
    PagingSystem& pager_;
    page_number root_;
};



} // indexing
} // paganini


#endif // __PAGANINI_INDEXING_BTREE_H__
