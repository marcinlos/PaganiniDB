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
private:
    typedef types::FieldMetadata Metadata;
    std::unordered_map<types::FieldType, Metadata> types;

    friend class util::Singleton<FieldFactory>;
    FieldFactory();
    
public:
    typedef std::unique_ptr<types::Data> DataPtr;
    typedef types::FieldMetadata::Creator Creator;
 
    // Zwraca obiekt pola o podanym typie . Rzuca wyjatek, jesli nie
    // mozna stworzyc takiego obiektu. Argument size jest opcjonalny,
    // podawac go nalezy dla typow o stalej, acz nie znanej na etapie
    // kompilacji dlugosci.
    DataPtr create(types::FieldType type, size16 size = 0) const;
    
    // Zwraca informacje o typie
    const Metadata& getMetadata(types::FieldType type) const;
    const Metadata& operator [](types::FieldType type) const;
    
    // Zmienia funkcje tworzaca dla podanego typu. Zwraca poprzednia.
    Creator registerCreator(types::FieldType type, Creator creator);
};


}


#endif // __PAGANINI_ROW_FIELD_FACTORY_H__
