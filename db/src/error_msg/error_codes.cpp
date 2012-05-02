#include "config.h"
#include "pdb_error.h"
#include <unordered_map>
#include <string>
#include <cstddef>
using std::string;

namespace paganini
{

static Error error_code;


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
    m[Error::NONE] =        "No error";
    m[Error::ARMAGEDON] =   "OMG blood everywhere";
    m[Error::FILECREATE] =  "Cannot create file";
    m[Error::FILEOPEN] =    "Cannot open file";
    m[Error::READ] =        "Read error";
    m[Error::SEEK] =        "Positioning error";
}

Error pdbErrno()
{
    return error_code;
}

int _pdbSetErrno(Error code)
{
    error_code = code;
}

const char* pdbErrorMsg(Error code)
{
    return messages.get(code);
}


}

