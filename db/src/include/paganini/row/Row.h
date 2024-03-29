/*
    Klasa wiersza tabeli bazy danych, odpowiedzialna za zarzadzanie 
    jego danymi. Jest iterowalna, ma ladnie poprzeciazane operatory
    tablicowe i w ogole. Umozliwia dostep do pol przez indeks lub
    nazwe. Dane przechowywane sa jako shared pointery.
*/
#ifndef __PAGANINI_ROW_ROW_H__
#define __PAGANINI_ROW_ROW_H__

#include <paganini/row/datatypes.h>
#include <paganini/row/RowFormat.h>

#include <paganini/inspect/primitives.h>

#include <vector>
#include <memory>
#include <iostream>


namespace paganini
{


class Row
{
public:
    // Definicje typow wskaznikowych zwracanych przez funkcje udostepniajace
    // pola wiersza
    typedef std::shared_ptr<types::Data> DataPtr;
    typedef std::shared_ptr<const types::Data> ConstDataPtr;
    
private:
    typedef std::vector<DataPtr> container;
    
public:
    // Typ zwracany przez funkcje udostepniajace 'widok' podzbioru wiersza
    typedef util::IndexedView<std::vector<int>::const_iterator,
        std::vector<DataPtr>::const_iterator> FieldPtrVector;
        
    // Definicje iteratorow do iterowania po polach wiersza
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    
    typedef std::shared_ptr<const RowFormat> FormatPtr;
    
    // Proxy do pol - zapobiega ustawieniu nieprawidlowych pol
    class FieldProxy;
    
    // Pusty, niezdatny do uzycia wiersz
    Row();
    
    // Inicjalizuje wiersz wskaznikami do danych
    Row(FormatPtr format, const std::vector<types::Data*>& fields, 
        row_flags flags = 0);
    
    // Inicjalizuje wiersz informacja o formacie, bez wartosci
    Row(FormatPtr format, row_flags flags = 0);
    
    // Copy constructor
    Row(const Row& other);
    
    // Move constructor
    Row(Row&& other);
    
    // Operator przypisania
    Row& operator = (const Row& other);
    
    // Operator move-przypisania
    Row& operator = (Row&& other);
    
    // Zwraca nullptr jesli wiersz jest poprawny, false jesli jest pusty.
    // To + operator ! ze wzgledu na fakt, ze mamy przeciazony rowniez
    // [] dla stringa, i wywolanie row["string"] moze odwolywac sie tak
    // do niego, jak i wbudowanego, choc malo znanego, operatora postaci
    // offset[adres].
    operator const void* () const;
    bool operator ! () const;
    
    // Ustawia podane pole na zadana wartosc
    void setField(size16 index, types::Data* data);   
    
    // Zwraca format wiersza
    const RowFormat& format() const;
    
    // Zwraca flagi wiersza
    row_flags flags() const;
    
    // Aby typ byl iterowalny
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
      
    // Zwraca ilosc kolumn 
    size16 columnCount() const;
    
    // Zwraca ilosc kolumn o stalym rozmiarze
    size16 fixedColumnCount() const;
    
    // Zwraca ilosc kolumn o zmiennym rozmiarze
    size16 variableColumnCount() const;
    
    // Zwraca wektor wskaznikow do wszystkich kolumn
    const std::vector<DataPtr>& columns() const;
    
    // Zwraca wektor wskaznikow do pol o stalym rozmiarze
    FieldPtrVector fixed() const;
    
    // Zwraca wektor wskaznikow do kolumn o zmiennym rozmiarze
    FieldPtrVector variable() const;
       
    // Zwraca numer kolumny o podanej nazwie, badz NULL_COLUMN, gdy 
    // kolumna o takiej nazwie nie istnieje.
    column_number getColumnNumber(const string& name) const;
    
    // Zwraca informacje, czy podane pole jest NULL-em
    bool isNull(const string& name) const;
    bool isNull(column_number col) const;
    
    // Zwraca wskaznik na dane z kolumny o podanej nazwie. 
    // Rzuca std::logic_error, jesli kolumna o takiej nazwie nie istnieje.
    FieldProxy operator [] (const string& name);
    
    ConstDataPtr operator [] (const string& name) const;
    
    // Zwraca wskaznik do danych w kolumnie o podanym numerze.
    FieldProxy operator [] (column_number col);
    
    ConstDataPtr operator [] (column_number col) const;
    
private:
    void swap_(Row&& other);

    FormatPtr format_;   
    container fields_;
    row_flags flags_;    
};

// Proxy do pol - zapobiega ustawieniu nieprawidlowych pol
class Row::FieldProxy
{
private:
    Row& row;
    const int index;

public:
    FieldProxy(Row& row, size16 index): row(row), index(index) { }
    operator DataPtr () { return row.fields_[index]; }
    FieldProxy& operator = (types::Data* data)
    {
        row.setField(index, data);
        return *this;
    }
    DataPtr operator -> () const { return row.fields_[index]; }
    types::Data& operator * () const { return *(row.fields_[index]); }
};


inline std::ostream& operator << (std::ostream& os, const Row& row)
{
    for (const Column& column: row.format())
    {
        auto data = row[column.col];
        os << column << ": " << (data ? data->toString() : "NULL") 
            << std::endl;
    }
    return os;
}


}


#endif // __PAGANINI_ROW_ROW_H__
