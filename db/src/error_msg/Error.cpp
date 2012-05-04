#include "config.h"
#include "Error.h"
#include <unordered_map>
#include <string>
#include <cstddef>
using std::string;

namespace paganini
{

// Mala, nieuzywana nigdzie klasa - mapa jest w nia opakowana tylko
// po to, by mozna w konstruktorze ja wypelnic.
class ErrorMessages
{
    std::unordered_map<Error, string> m;

public:    
    ErrorMessages();
    
    const char* get(Error code) 
    {
        auto i = m.find(code);
        if (i != m.end())
            return i->second.c_str();
        else 
            return nullptr;
    }
} 
messages;

ErrorMessages::ErrorMessages()
{
    m[Error::NONE] =            "No error";
    m[Error::ARMAGEDON] =       "OMG blood everywhere";
    m[Error::FILECREATE] =      "Cannot create file";
    m[Error::FILEOPEN] =        "Cannot open file";
    m[Error::READ] =            "Read error";
    m[Error::SEEK] =            "Positioning error";
    m[Error::DATA_TOO_LONG] =   "Given data is too long for this type";
}


const char* get_error_message(Error code)
{
    return messages.get(code);
}


}

