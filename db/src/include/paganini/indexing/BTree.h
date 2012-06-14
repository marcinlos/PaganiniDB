/*

    Specyfikacja typow udostepnianych przez parametry szablonu:
      _DataPage::RowType 
         Typ podstawowy wiersza danych. Jest przyjmowany jako argument przy 
         wstawianiu, powinny przyjmowac go funkcje porownujace _Indexer-a, 
         przyjmowac do zapisu _DataPage. Jest zwracany przez funkcje 
         wyszukujace.
         
      _IndexPage::RowType
         Typ podstawowy indeksu. Jest przyjmowany przez funkcje porownujace
         _Indexer-a, oraz jako argument zapisu dla _IndexPage.
*/
#ifndef __PAGANINI_INDEXING_BTREE_H__
#define __PAGANINI_INDEXING_BTREE_H__

#include <paganini/paging/types.h>
#include <memory>
#include <vector>

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
    typedef typename _DataPage::RowType RowType;
    
    struct RowIterator
    {
        page_number page;
        row_number row;
        
        RowIterator(page_number page = NULL_PAGE, row_number row = 0):
            page(page), row(row)
        {
        }
        
        operator bool () const
        {
            return page != NULL_PAGE;
        }
    };
    
    // Tworzy nowe B+ drzewo oparte na podanym systemie stronnicowania.
    // Jego korzen jest zapisywany do strony o numerze root. Jesli
    // root == ALLOC_NEW, strona jest pobierana poprzez metode allocPage
    // systemu stronnicowania.
    explicit BTree(_PagingSystem& pager, page_number root = ALLOC_NEW,
        const _Indexer& indexer = _Indexer());
       
       
    // Zwraca numer strony korzenia
    page_number root() const
    {
        return root_;
    }
    
    
    // Dodaje wiersz do drzewa
    bool insert(const RowType& row);
    
    // Wyszukuje wiersz
    RowType find(const IndexType& key) const
    {
        RowIterator iter = findFirst(key);
        _DataPage page;
        if (iter)
        {
            pager_.readPage(iter.page, page.buffer());
            return page.row(iter.row);
        }
        else
            return RowType();
    }
    
    template <typename _OutIter>
    void readRange(RowIterator first, const IndexType& key, _OutIter out) const
    {
        _DataPage page;
        page_number page_num = first.page;

        pager_.readPage(page_num, page.buffer());
        for (int i = first.row; i < page.rowCount(); ++ i)
        {
            RowType row = page.row(i, indexer_.rowFormat());
            IndexType index = indexer_(row);
            if (indexer_(key, index) != 0)
                break;
            *out ++ = row;
        }
        page_num = page.next();
        while (page_num != NULL_PAGE)
        {
            pager_.readPage(page_num, page.buffer());
            for (unsigned i = 0; i < page.rowCount(); ++ i)
            {
                RowType row = page.row(i, indexer_.rowFormat());
                IndexType index = indexer_(row);
                if (indexer_(key, index) != 0)
                    return;
                *out ++ = row;
            }
        }
    }
     
    RowIterator findFirst(const IndexType& key) const
    {
         _DataPage page;
        page_number page_num = root_;
        while (true)
        {
            pager_.readPage(page_num, page.buffer());
            
            for (size16 i = 0; i < page.rowCount(); ++ i)
            {
                RowType row = page.row(i, indexer_.rowFormat());
                IndexType index = indexer_(row);
                
                if (indexer_(key, index) == 0)
                    return { page_num, i }; 
            }
            if (page_num == end_)
                break;
            else
                page_num = page.next();
        }  
        return { NULL_PAGE, 0 };
    }
    
    template <typename _OutIter>
    void fetchAll(_OutIter out) const
    {
        std::vector<RowType> rows;
        _DataPage page;
        page_number page_num = root_;
        while (true)
        {
            pager_.readPage(page_num, page.buffer());
            
            for (unsigned i = 0; i < page.rowCount(); ++ i)
                *out ++ = page.row(i, indexer_.rowFormat());

            if (page_num == end_)
                break;
            else
                page_num = page.next();
        }
    }
    
    template <typename _OutIter>
    void findRange(const IndexType& key, _OutIter out) const
    {
        RowIterator i = findFirst(key);
        if (i)
            readRange(i, key, out);
    }
    
    const _Indexer& indexer() const { return indexer_; }

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
    page.insert(row, 0);
}




}


#endif // __PAGANINI_INDEXING_BTREE_H__
