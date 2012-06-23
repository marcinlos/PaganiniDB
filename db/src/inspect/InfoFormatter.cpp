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
    ss << left << setw(15) << "Page number: " << setw(13) << right
        << page_number_str(header.number) << std::endl;
        
    ss << left << setw(15) << "Type: " << setw(13) << right
        << get_page_type(header.flags) << std::endl;
        
    ss << left << setw(15) << "Flags: " << setw(13) << right
        << page_flags_str(header.flags) << std::endl;  
        
    ss << left << setw(15) << "Prev: " << setw(13) << right
        << page_number_str(header.prev) << std::endl;
        
    ss << left << setw(15) << "Next: " << setw(13) << right
        << page_number_str(header.next) << std::endl;
        
    ss << left << setw(15) << "Owner: " << setw(13) << right
        << object_id_str(header.owner) << std::endl;
        
    ss << left << setw(15) << "Rows: " << setw(13) << right
        << header.rows << std::endl;
        
    ss << left << setw(15) << "Free space: " << setw(13) << right
        << header.free_space << std::endl;
        
    ss << left << setw(15) << "Free offset: " << setw(13) << right
        << header.free_offset << std::endl;   
               
    return ss.str();
}


string InfoFormatter::operator ()(const DatabaseHeader& header) const
{
    using namespace std;
    ostringstream ss;
    ss << left << setw(15) << "Database name: " << setw(25) << right 
        << header.db_name << std::endl;
        
    ss << left << setw(15) << "Page count: " << setw(25) << right 
        << header.page_count << std::endl;
    
    static const int BUFFER_SIZE = 1024;
    struct tm* t = gmtime(&header.creation_time);
    char buffer[BUFFER_SIZE];
    strftime(buffer, BUFFER_SIZE, "%H:%M:%S %d.%m.%G", t); 
    
    ss << left << setw(15) << "Creation time: " << setw(25) << right 
        << buffer << std::endl;
        
    return ss.str();
}


string InfoFormatter::operator ()(const PageBuffer& buffer, int bytes) const
{
    std::ostringstream ss;
    ss << (*this)(buffer.header) << std::endl;
    if (bytes == -1)
        bytes = DATA_SIZE;

        
    if (bytes > 0)
    {
        ss << format_bytes(buffer.data, bytes);
    }
    return ss.str();
}


string InfoFormatter::uv(const PageBuffer& buffer) const
{
    std::ostringstream ss;
    
    for (unsigned int i = 0; i < PAGES_PER_UV / 8; ++ i)
    {
        if (i % 8 == 0)
            ss << std::endl;
        for (int mask = 1; mask < (1 << 8); mask <<= 1)
        {
            ss.put((buffer.data[i] & mask) ? '*' : '.');
        }
        ss.put(' ');
    }
    ss << std::endl;
    return ss.str();
}


} // inspect
} // paganini

