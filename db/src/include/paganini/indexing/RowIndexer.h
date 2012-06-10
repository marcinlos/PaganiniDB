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
    typedef std::unique_ptr<IndexType> ReturnType;
    typedef Row RowType;
    typedef RowFormat FormatInfo;
    
    // Tworzy indeks na kolumne o podanym numerze
    RowIndexer(const RowFormat& fmt, column_number column);    
    
    // Tworzy indeks na kolumne o podanej nazwie
    RowIndexer(const RowFormat& fmt, const string& name);
    
    // Tworzy indeks na podstawie podanego wiersza i numeru strony, do
    // ktorej ma odnosic
    ReturnType operator ()(const Row& a, page_number dest) const;
    
    // Porownywanie dwoch wierszy
    int operator ()(const Row& a, const Row& b) const;
    
    // Porownywanie dwoch indeksow
    int operator ()(const Index& a, const Index& b) const;
    
    

private:
    void fromNumber_(const RowFormat& fmt, column_number column);
    
    // Zwykly wskaznik, bo korzystamy z fabryki ktora je flyweightuje
    Comparator<types::Data>* comparator_;
    column_number column_;
    types::FieldType type_;
};


}


#endif // __PAGANINI_INDEXING_ROW_INDEXER_H__
