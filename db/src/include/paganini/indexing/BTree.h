/*
*/
#ifndef __PAGANINI_INDEXING_BTREE_H__
#define __PAGANINI_INDEXING_BTREE_H__

#include <paganini/paging/types.h>
#include <memory>

#include <iostream>


namespace paganini
{

template <
    class _PagingSystem,
    class _Indexer,
    class _IndexPage,
    class _DataPage
    >
class BTree
{
public:
    static const page_number ALLOC_NEW = -1;
    
    typedef typename _IndexPage::RowType IndexType;
    typedef typename _IndexPage::FormatInfo IndexFormatInfo;
    typedef typename _DataPage::RowType RowType;
    typedef typename _DataPage::FormatInfo RowFormatInfo;
    
    // Tworzy nowe B+ drzewo oparte na podanym systemie stronnicowania.
    // Jego korzen jest zapisywany do strony o numerze root. Jesli
    // root == ALLOC_NEW, strona jest pobierana poprzez metode allocPage
    // systemu stronnicowania.
    explicit BTree(_PagingSystem& pager, page_number root = ALLOC_NEW,
        const _Indexer& indexer = _Indexer());
    
    // Dodaje wiersz do drzewa
    bool insert(const RowType& row);

private:

    void insertDataNonfull_(const RowType& row, _DataPage& page);

    _PagingSystem& pager_;
    page_number root_, end_;
    _Indexer indexer_;
};


template <class _PagingSystem, class _Indexer, class _IndexPage, class _DataPage>
BTree<_PagingSystem, _Indexer, _IndexPage, _DataPage>::BTree(
    _PagingSystem& pager, page_number root, const _Indexer& indexer):
    pager_(pager), root_(root), indexer_(indexer)
{
    if (root_ == ALLOC_NEW)
        root_ = pager_.allocPage();
    
    std::cout << "Root = " << root_ << std::endl;
    end_ = root_;
    _DataPage root_page;
    pager_.readPage(root_, root_page.buffer());
    root_page.setNumber(root_);
    pager_.writePage(root_, root_page.buffer());
}


template <class _PagingSystem, class _Indexer, class _IndexPage, class _DataPage>
bool BTree<_PagingSystem, _Indexer, _IndexPage, 
    _DataPage>::insert(const RowType& row)
{
    _DataPage page;
    pager_.readPage(end_, page.buffer());
    if (! page.canFit(row))
    {
        page_number prev = end_;
        end_ = pager_.allocPage();
        page.setNext(end_);
        pager_.writePage(prev, page.buffer());
        
        pager_.readPage(end_, page.buffer());
        page.setPrev(prev);
    }
    insertDataNonfull_(row, page);
    pager_.writePage(end_, page.buffer());
    return true;
}


template <class _PagingSystem, class _Indexer, class _IndexPage, class _DataPage>
void BTree<_PagingSystem, _Indexer, _IndexPage, 
    _DataPage>::insertDataNonfull_(const RowType& row, _DataPage& page)
{
    std::cout << "Wstawiamy na " << page.number() << std::endl;
    page.insert(row, 0);
}



}


#endif // __PAGANINI_INDEXING_BTREE_H__
