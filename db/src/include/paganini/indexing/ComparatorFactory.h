/*
    Realizuje dynamiczne tworzenie komparatorow na podstawie przekazanego
    typu pola, ktore nalezy porownywac.
*/
#ifndef __PAGANINI_INDEXING_COMPARATOR_FACTORY_H__
#define __PAGANINI_INDEXING_COMPARATOR_FACTORY_H__

#include <paganini/row/datatypes.h>
#include <paganini/util/Singleton.h>
#include <paganini/row/Comparator.h>
#include <unordered_map>
#include <memory>


namespace paganini
{

class ComparatorFactory: public util::Singleton<ComparatorFactory>
{
public:    
    // Typ zwracany przez funkcje tworzaca
    typedef Comparator<types::Data>* ComparatorPointer;

    // Funkcja zwracajaca komparator dla podanego typu
    ComparatorPointer get(types::ContentType type) const;
    
private:
    ComparatorFactory();
    
    friend class util::Singleton<ComparatorFactory>;
        
    // Domyslna polityka rzutowania uzywana przez comparatory
    typedef DynamicCastPolicy CastingPolicy;
    
    typedef std::unique_ptr<Comparator<types::Data>> ContainedPointer;
    std::unordered_map<types::ContentType, ContainedPointer> comparators_;
};


}


#endif // __PAGANINI_INDEXING_COMPARATOR_FACTORY_H__
