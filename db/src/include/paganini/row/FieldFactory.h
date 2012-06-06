/*
    Fabryka pol bazodanowych. Umozliwia tworzenie obiektow przechowujacych
    dane na podstawie podanego typu pola w oparciu o customizowalne
    funkcje tworzace.
*/
#ifndef __PAGANINI_ROW_FIELD_FACTORY_H__
#define __PAGANINI_ROW_FIELD_FACTORY_H__

#include <paganini/row/datatypes.h>
#include <paganini/util/Singleton.h>

#include <memory>
#include <functional>
#include <unordered_map>

namespace paganini
{


class FieldFactory: public util::Singleton<FieldFactory>
{
public:
    // Funkcja tworzaca obiekt danej
    typedef std::function<types::Data* (size16)> Creator;
    
    // Typ wskaznika zwracanego przez metode tworzaca
    typedef std::unique_ptr<types::Data> DataPtr;
    
    // Zwraca obiekt pola o podanym typie . Rzuca wyjatek, jesli nie
    // mozna stworzyc takiego obiektu.
    DataPtr create(types::FieldType type) const;
    
    // Zwraca rozmiar (w bajtach) fizycznej reprezentacji danego typu,
    // lub types::VARIABLE_SIZE jesli typ jest zmiennej dlugosci.
    size16 size(types::FieldType type) const;

    // Zmienia funkcje tworzaca dla podanego typu. Zwraca poprzednia.
    Creator registerCreator(types::ContentType type, Creator creator);
    
private:
    struct FieldMetadata
    {       
        Creator creator;
        std::function<size16 (size16)> size;
    };

    std::unordered_map<types::ContentType, FieldMetadata> types;
    friend class util::Singleton<FieldFactory>;
    
    FieldFactory();
};


}


#endif // __PAGANINI_ROW_FIELD_FACTORY_H__
