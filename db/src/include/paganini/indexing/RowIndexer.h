/*
*/
#ifndef __PAGANINI_INDEXING_ROW_INDEXER_H__
#define __PAGANINI_INDEXING_ROW_INDEXER_H__

#include <paganini/row/RowFormat.h>
#include <paganini/row/Row.h>
#include <paganini/indexing/Index.h>
#include <paganini/row/Comparator.h>
#include <string>
#include <memory>
using std::string;


namespace paganini
{


class RowIndexer
{
public:
    // Typedefy wymagane przez BTree
    typedef Index IndexType;
    typedef std::unique_ptr<Index> IndexPtr;
    typedef std::unique_ptr<const Index> ConstIndexPtr;
    typedef IndexPtr IndexReturnType;

    
    typedef std::shared_ptr<const RowFormat> FormatPtr;

    typedef FormatPtr RowFormatInfo;
    typedef types::FieldType IndexFormatInfo;
    
    typedef Row RowType;
    typedef std::unique_ptr<Row> RowPtr;
    typedef std::unique_ptr<const Row> ConstRowPtr;
    typedef RowPtr RowReturnType;
    
    // Tworzy indeks na kolumne o podanym numerze
    RowIndexer(FormatPtr fmt, column_number column);    
    
    // Tworzy indeks na kolumne o podanej nazwie
    RowIndexer(FormatPtr fmt, const string& name);
    
    // Zwraca informacje o formacie wiersza
    inline RowFormatInfo rowFormat() const;
    
    // Zwraca informacje o formacie indeksu
    inline IndexFormatInfo indexFormat() const;
    
    // Tworzy indeks na podstawie podanego wiersza i numeru strony, do
    // ktorej ma odnosic
    IndexReturnType operator ()(const Row& a, page_number dest = 0) const;
    
    // Porownywanie dwoch wierszy
    int operator ()(const Row& a, const Row& b) const;
    
    // Porownywanie dwoch indeksow
    int operator ()(const Index& a, const Index& b) const;
    
    

private:
    void fromNumber_(FormatPtr fmt, column_number column);
    
    // Zwykly wskaznik, bo korzystamy z fabryki ktora je flyweightuje
    Comparator<types::Data>* comparator_;
    column_number column_;
    types::FieldType type_;
    FormatPtr format_;
};


RowIndexer::RowFormatInfo RowIndexer::rowFormat() const
{
    return format_;
}


RowIndexer::IndexFormatInfo RowIndexer::indexFormat() const
{
    return type_;
}


}


#endif // __PAGANINI_INDEXING_ROW_INDEXER_H__
