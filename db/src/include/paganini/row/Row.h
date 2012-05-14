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
#include <vector>
#include <memory>


namespace paganini
{


class Row
{
private:
    typedef std::shared_ptr<types::Data> DataPtr;
    typedef std::shared_ptr<const types::Data> ConstDataPtr;
    typedef std::vector<DataPtr> container;
    
    const RowFormat& _format;   
    container _fields;
    row_flags _flags;

public:
    typedef util::IndexedView<std::vector<int>::const_iterator,
        std::vector<DataPtr>::const_iterator> FieldPtrVector;
        
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    
    // Proxy do pol - zapobiega ustawieniu nieprawidlowych pol
    class FieldProxy
    {
        Row& row;
        const size16 index;
        
    public:
        FieldProxy(Row& row, size16 index): row(row), index(index) { }
        operator DataPtr () { return row._fields[index]; }
        FieldProxy& operator = (types::Data* data)
        {
            row.setField(index, data);
            return *this;
        }
        DataPtr operator -> () const { return row._fields[index]; }
        types::Data& operator * () const { return *(row._fields[index]); }
    };
    

    // Inicjalizuje wiersz wskaznikami do danych
    Row(const RowFormat& format, std::initializer_list<types::Data*> fields,
        row_flags flags = 0);
    
    // Inicjalizuje wiersz informacja o formacie, bez wartosci
    Row(const RowFormat& format, row_flags flags = 0);
    
    // Move constructor
    Row(Row&& other);
    
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
};


}


#endif // __PAGANINI_ROW_ROW_H__
