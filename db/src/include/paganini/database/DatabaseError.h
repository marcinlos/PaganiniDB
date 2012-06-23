/*
    Klasa wyjatku dla silnika bazy danych
*/
#ifndef __PAGANINI_DATABASE_DATABASE_ERROR_H__
#define __PAGANINI_DATABASE_DATABASE_ERROR_H__

#include <stdexcept>
#include <string>
using std::string;


namespace paganini
{


class DatabaseError: virtual public std::runtime_error
{
public:
    DatabaseError(const string& description): 
        std::runtime_error(description)
    {
    }
};

} // paganini


#endif // __PAGANINI_DATABASE_DATABASE_ERROR_H__
