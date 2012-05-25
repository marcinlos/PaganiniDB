/*
    Fabryka pol bazodanowych. Umozliwia tworzenie obiektow przechowujacych
    dane na podstawie podanego typu pola w oparciu o customizowalne
    funkcje tworzace. Udostepnia rowniez dane o samych typach.
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
    typedef std::function<types::Data* (size16)> Creator;
    typedef std::unique_ptr<types::Data> DataPtr;
    
private:
    struct FieldMetadata
    {       
        Creator creator;
        std::function<size16 (size16)> size;
        
        types::Data* operator () (size16 size = 0) const
        {
            return creator(size);
        }
    };

    std::unordered_map<types::ContentType, FieldMetadata> types;

    friend class util::Singleton<FieldFactory>;
    FieldFactory();
    
public:
    // Zwraca obiekt pola o podanym typie . Rzuca wyjatek, jesli nie
    // mozna stworzyc takiego obiektu.
    DataPtr create(types::FieldType type) const;
    
    // Zwraca rozmiar (w bajtach) fizycznej reprezentacji danego typu,
    // lub types::VARIABLE_SIZE jesli typ jest zmiennej dlugosci.
    size16 size(types::FieldType type) const;

    // Zmienia funkcje tworzaca dla podanego typu. Zwraca poprzednia.
    Creator registerCreator(types::ContentType type, Creator creator);
};


}


#endif // __PAGANINI_ROW_FIELD_FACTORY_H__
