/*

    Specyfikacja typow udostepnianych przez parametry szablonu:
      _DataPage::RowType 
         Typ podstawowy wiersza danych. Jest przyjmowany jako argument przy 
         wstawianiu, powinny przyjmowac go funkcje porownujace _Indexer-a, 
         przyjmowac do zapisu _DataPage.
         
      _DataPage::ReturnRowType
         Typ 'wskaznikowy' wiersza danych. Jest zwracany przez funkcje
         _DataPage::row. Rezultat jego dereferencji powinien byc mozliwy do
         uzywania jako _DataPage::RowType. Jest rowniez zwracany jako wynik
         wyszukiwania. Jego domyslna wartosc jest zwracana, gdy wiersz
         nie zostal odnaleziony.
         
      _IndexPage::RowType
         Typ podstawowy indeksu. Jest przyjmowany przez funkcje porownujace
         _Indexer-a, oraz jako argument zapisu dla _IndexPage.
         
      _IndexPage::ReturnType
         Typ 'wskaznikowy' wiersza danych. Jest zwracany przez _IndexPage::row
         oraz przez operator wywolania _Indexer-a. Jego dereferencja jest 
         uzywana tam, gdzie potrzebny jest _IndexPage::RowType.
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
    typedef typename _IndexPage::ReturnType IndexReturnType;
    typedef typename _DataPage::RowType RowType;
    typedef typename _DataPage::ReturnType RowReturnType;
    
    
    // Tworzy nowe B+ drzewo oparte na podanym systemie stronnicowania.
    // Jego korzen jest zapisywany do strony o numerze root. Jesli
    // root == ALLOC_NEW, strona jest pobierana poprzez metode allocPage
    // systemu stronnicowania.
    explicit BTree(_PagingSystem& pager, page_number root = ALLOC_NEW,
        const _Indexer& indexer = _Indexer());
    
    // Dodaje wiersz do drzewa
    bool insert(const RowType& row);
    
    // Wyszukuje wiersz
    RowReturnType find(const IndexType& key) const;
    
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


template <class _PagingSystem, class _Indexer, class _IndexPage, class _DataPage>
typename BTree<_PagingSystem, _Indexer, _IndexPage, _DataPage>::RowReturnType
BTree<_PagingSystem, _Indexer, _IndexPage, 
    _DataPage>::find(const IndexType& key) const
{
    _DataPage page;
    page_number page_num = root_;
    while (true)
    {
        pager_.readPage(page_num, page.buffer());
        
        for (unsigned i = 0; i < page.rowCount(); ++ i)
        {
            RowReturnType row = page.row(i, indexer_.rowFormat());
            IndexReturnType index = indexer_(*row);
            
            if (indexer_(key, *index) == 0)
            {
                return row; 
            }
        }
        if (page_num == end_)
            break;
        else
            page_num = page.next();
    }
    return RowReturnType();
}


}


#endif // __PAGANINI_INDEXING_BTREE_H__
