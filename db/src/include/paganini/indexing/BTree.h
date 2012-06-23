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
        const _Indexer& indexer = _Indexer()):
        pager_(pager), root_(root), indexer_(indexer)
    {
        if (root_ == ALLOC_NEW)
            root_ = pager_.allocPage();
        
        end_ = root_;
        _DataPage root_page;
        auto writeLock = pager_.writeLock(root_);
        pager_.readPage(root_, root_page.buffer());
        root_page.setNumber(root_);
        pager_.writePage(root_, root_page.buffer());
    }
    
    
    // Noncopyable
    BTree(const BTree&) = delete;
    
    
    // Move-constructor
    BTree(BTree&& other): pager_(other.pager_), root_(other.root),
        end_(other.end_), indexer_(std::move(other.indexer_))
    {
        other.root_ = NULL_PAGE;
    }
    
       
    // Zwraca numer strony korzenia
    page_number root() const
    {
        return root_;
    }
    
    
    // Dodaje wiersz do drzewa
    bool insert(const RowType& row)
    {
        auto writeLock = pager_.writeLock(end_);
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
        // Dla uproszczenia korzystamy z rekursywnosci
        auto secondLock = pager_.writeLock(end_);
        pager_.writePage(end_, page.buffer());
        sleep(10);
        return true;
    }
    
    
    // Wyszukuje wiersz pasujacy do podanego predykatu. Szczegoly nizej w
    // findAll.
    template <typename _Predicate>
    Row find(_Predicate matches) const
    {
        _DataPage page;
        page_number page_num = root_;
        while (true)
        {
            auto readLock = pager_.readLock(page_num);
            pager_.readPage(page_num, page.buffer());
            
            for (size16 i = 0; i < page.rowCount(); ++ i)
            {
                RowType row = page.row(i, indexer_.rowFormat());
                
                if (matches(row))
                    return row; 
            }
            if (page_num == end_)
                break;
            else
                page_num = page.next();
        }
        return Row();
    }
    
    
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
    
    
    // Wyszukuje wszystkie wiersze pasujace do podanego predykatu. Obiekt 
    // predykatu powinien posiadac operator () przyjmujacy typ Row i
    // zwracajacy wartosc konwertowalna na bool. 
    template <typename _Predicate, typename _OutIter>
    void findAll(_Predicate matches, _OutIter out) const
    {
         _DataPage page;
        page_number page_num = root_;
        while (true)
        {
            auto readLock = pager_.readLock(page_num);
            pager_.readPage(page_num, page.buffer());
            
            for (size16 i = 0; i < page.rowCount(); ++ i)
            {
                RowType row = page.row(i, indexer_.rowFormat());
                
                if (matches(row))
                    *out ++ = row; 
            }
            if (page_num == end_)
                break;
            else
                page_num = page.next();
        }
    }
    
    
    // Wyszukuje wszystkie pasujace wiersze brutalnie przegladajac cala tabele
    template <typename _OutIter>
    void findAll(const IndexType& key, _OutIter out) const
    {
        // Forwardujemy do funkcji ogolnej przyjmujacej predykat
        findAll([this, key](const Row& row)
        {
            IndexType index = indexer_(row);
            return indexer_(key, index) == 0;
        }, 
        out);
    }
    
    
    // Wyszukuje wszystkie pasujace wiersze od first do pierwszego 
    // niepasujacego
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
    
     
    // Znajduje pierwszy pasujacy wiersz, zwraca jego pozycjje
    RowIterator findFirst(const IndexType& key) const
    {
         _DataPage page;
        page_number page_num = root_;
        while (true)
        {
            auto readLock = pager_.readLock(page_num);
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
    
    
    // Zwraca wszystkie wiersze (zapisuje przy uzyciu iteratora out)
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
    
    
    // Wyszukuje pasujace wiersze od pierwszego pasujacego do pierwszego
    // niepasujacego po nim
    template <typename _OutIter>
    void findRange(const IndexType& key, _OutIter out) const
    {
        RowIterator i = findFirst(key);
        if (i)
            readRange(i, key, out);
    }
    
    
    // Zwraca wiersz o najwiekszym indeksie
    RowType max()
    {
        // Nie chcemy by ktokolwiek zmienial zawartosc 
        auto readLock = pager_.readLock(root_);
        _DataPage page;
        page_number page_num = root_;
        RowType current_max;
        IndexType index_max;
        bool empty = true;
        while (true)
        {
            pager_.readPage(page_num, page.buffer());
            
            for (size16 i = 0; i < page.rowCount(); ++ i)
            {
                RowType row = page.row(i, indexer_.rowFormat());
                IndexType index = indexer_(row);
                if (empty)
                {
                    current_max = row;
                    index_max = index;
                    empty = false;
                }
                else if (indexer_(index_max, index) < 0)
                {
                    current_max = row;
                    index_max = index;
                }
            }
            if (page_num == end_)
                break;
            else
                page_num = page.next();
        }
        return current_max;
    }
    
    const _Indexer& indexer() const { return indexer_; }

private:

    void insertDataNonfull_(const RowType& row, _DataPage& page)
    {
        page.insert(row, 0);
    }

    _PagingSystem& pager_;
    page_number root_, end_;
    _Indexer indexer_;
};


}


#endif // __PAGANINI_INDEXING_BTREE_H__

