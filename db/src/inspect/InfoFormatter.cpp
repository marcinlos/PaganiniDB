#include "config.h"
#include <paganini/inspect/InfoFormatter.h>
#include <paganini/inspect/format_bytes.h>


namespace paganini
{

namespace inspect
{


string InfoFormatter::operator ()(const PageHeader& header) const
{
    using namespace std;
    ostringstream ss;
    ss << left << setw(15) << "Page number: " << setw(13) 
        << page_number_str(header.number) << std::endl;
        
    ss << left << setw(15) << "Type: " << setw(13) 
        << get_page_type(header.flags) << std::endl;
        
    ss << left << setw(15) << "Flags: " << setw(13) 
        << page_flags_str(header.flags) << std::endl;  
        
    ss << left << setw(15) << "Prev: " << setw(13) 
        << page_number_str(header.prev) << std::endl;
        
    ss << left << setw(15) << "Next: " << setw(13) 
        << page_number_str(header.next) << std::endl;
        
    ss << left << setw(15) << "Owner: " << setw(13) 
        << object_id_str(header.owner) << std::endl;
        
    ss << left << setw(15) << "Rows: " << setw(13) 
        << header.rows << std::endl;
        
    ss << left << setw(15) << "Free space: " << setw(13) 
        << header.free_space << std::endl;
        
    ss << left << setw(15) << "Free offset: " << setw(13) 
        << header.free_offset << std::endl;   
               
    return ss.str();
}


string InfoFormatter::operator ()(const PageBuffer& buffer, int bytes) const
{
    std::ostringstream os;
    os << (*this)(buffer.header) << std::endl;
    if (bytes == -1)
        bytes = DATA_SIZE;

        
    if (bytes > 0)
    {
        os << format_bytes(buffer.data, bytes);
    }
    return os.str();
}


} // inspect
} // paganini
