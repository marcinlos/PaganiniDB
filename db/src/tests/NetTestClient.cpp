#include "config.h"
#include <paganini/networking/address.h>
#include <paganini/networking/unix/Socket.h>
#include <string.h>
#include <string>
using std::string; 

using namespace paganini::networking;


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: net_sv <port>" << std::endl;
        exit(1);
    }
    try
    {
        std::stringstream ns(argv[1]);
        short port;
        ns >> port;
        address::Internet inet("127.0.0.1", port);
        unix::Socket<address::Internet> socket;
        socket.connect(inet);
        char buffer[10000+1] = {};
        for (;  ;)
        {
            int count = socket.receive(make_buffer(buffer, 10000));
            if (count == 0)
                break;
            buffer[count] = '\0';
            std::cout << "Message: " << string(buffer) << std::endl;
            std::cout << "Returned: " << count << std::endl;
        }
    }
    catch (NetworkingError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "Error: " << strerror(e.errorCode()) << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
