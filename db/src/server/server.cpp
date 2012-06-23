#include "config.h"
#include "Interpreter.h"
#include "DatabaseServer.h"
#include <paganini/database/DatabaseEngine.h>
#include <paganini/concurrency/SystemError.h>

using namespace paganini;

int main(int argc, char* argv[]) 
{
    //paganini::server::DatabaseServer server;
    bool read = true;
    if (argc >= 2 && strcmp(argv[1], "new") == 0)
        read = false;
    engine::DatabaseEngine engine("db", ! read);    
    paganini::shell::Interpreter interpreter(engine);
    
    std::string line;
    while (std::getline(std::cin, line))
    {
        try
        {
            interpreter.process(line);
        }
        catch (paganini::concurrency::SystemError& e)
        {
            std::cerr << e.what() << std::endl;
            std::cerr << "Error: " << strerror(e.errorCode()) << std::endl;
        }
        catch (paganini::Exception& e)
        {
            std::cerr << e.what() << std::endl 
                << e.getCodeMessage() << std::endl;
        }
        catch (std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
    return 0;
}
