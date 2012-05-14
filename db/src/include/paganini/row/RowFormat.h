/*
    Opis danych przechowywanych w pojedynczym wierszu. Struktura Column 
    opisuje pojedyncza kolumne w wierszu - jej typ, flagi itd. 
    RowFormat przechowuje liste tych kolumn, i umozliwia dostep do nich
    w rozmaity sposob - przez indeks, nazwe itp.
*/
#ifndef __PAGANINI_ROW_ROW_FORMAT_H__
#define __PAGANINI_ROW_ROW_FORMAT_H__

#include <paganini/paging/types.h>
#include <paganini/row/datatypes.h>
#include <paganini/util/IndexedView.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iterator>
using std::string;


namespace paganini
{

// Opis pojedynczej kolumny
struct Column
{
    column_number col;
    types::FieldType type;
    column_flags flags;
    size16 size;
    string name;
    
    // Brak numeru kolumny - ustawiana przy wstawianiu do danych wiersza
    Column(types::FieldType type, string name, column_flags flags = 0,
        size16 size = 0);
    
    // Konstruktor przenoszacy, unika kopiowania stringa
    Column(Column&& other);
    
    // Zwyczajny konstruktor kopiujacy
    Column(const Column& other);
};



// Opis calego wiersza
class RowFormat
{
private:
    std::vector<Column> _columns;
    std::vector<int> _fixed;
    std::vector<int> _variable;
    std::unordered_map<string, int> _names;
    size16 _fixed_size;
    
public:

    typedef util::IndexedView<std::vector<int>::const_iterator,
        std::vector<Column>::const_iterator> ColumnVector;
        
    // typedef std::vector<Column>::iterator iterator;
    typedef std::vector<Column>::const_iterator const_iterator;

    // Inicjalizuje format wiersza z wektora opisow kolumn
    RowFormat(const std::vector<Column>& cols);
    
    // To samo z listy inicjaliztorow
    RowFormat(std::initializer_list<Column> cols);
    
    // Dodaje na koniec pojedyncza kolumne
    void addColumn(const Column& col);
    
    // Aby typ byl iterowalny
    const_iterator begin() const;
    const_iterator end() const;
    
    // Zwraca ilosc kolumn 
    size16 columnCount() const;
    
    // Zwraca ilosc kolumn o stalym rozmiarze
    size16 fixedColumnCount() const;
    
    // Zwraca laczna dlugosc pol o stalym rozmiarze
    size16 totalFixedSize() const;
    
    // Zwraca ilosc kolumn o zmiennym rozmiarze
    size16 variableColumnCount() const;
    
    // Zwraca wektor wszystkich kolumn
    const std::vector<Column>& columns() const;
    
    // Zwraca wektor kolumn o stalym rozmiarze
    ColumnVector fixed() const;
    
    // Zwraca wektor indeksow kolumn o stalym rozmiarze
    const std::vector<int>& fixedIndices() const;
    
    // Zwraca wektor kolumn o zmiennym rozmiarze
    ColumnVector variable() const;
    
    // Zwraca wektor indeksow kolumn o zmiennym rozmiarze
    const std::vector<int>& variableIndices() const;
    
    // Zwraca numer kolumny o podanej nazwie, badz NULL_COLUMN, gdy 
    // kolumna o takiej nazwie nie istnieje.
    column_number getColumnNumber(const string& name) const;
    
    // Zwraca dane o kolumnie o podanej nazwie. Rzuca std::logic_error, 
    // jesli kolumna o takiej nazwie nie istnieje.
    Column& operator [] (const string& name);
    
    const Column& operator [] (const string& name) const;
    
    // Zwraca dane o kolumnie o podanym numerze.
    Column& operator [] (column_number col);
    
    const Column& operator [] (column_number col) const;
};


}

#endif // __PAGANINI_ROW_ROW_FORMAT_H__
