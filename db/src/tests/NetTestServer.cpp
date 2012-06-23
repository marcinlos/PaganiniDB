#include "config.h"
#include <paganini/networking/address.h>
#include <paganini/networking/unix/Socket.h>
#include <paganini/networking/unix/Acceptor.h>
#include <paganini/networking/send.h>
#include <string.h>
#include <string>
#include <sstream>
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
        address::Internet inet("", port);
        /*unix::Socket<address::Internet> socket;
        socket.bind(inet);
        */
        unix::Acceptor<address::Internet> acceptor(inet);
        unix::Socket<address::Internet> socket = acceptor.accept();
        //char message[] = "Hello, World!";
        std::stringstream ss;
        for (int i = 0; i < 10000; ++ i)
            ss << i << ", ";
        //ss << 10;
        // socket.send(make_buffer(message));
        write(socket, ss.str());
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
