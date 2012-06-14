//#include <paganini/tools/dbshell/Interpreter.h>
#include "Interpreter.h"
#include <string>
#include <iostream>


int main(int argc, char* argv[])
{
    bool read = true;
    if (argc >= 2 && strcmp(argv[1], "new"))
        read = false;
    paganini::shell::Interpreter interpreter(read);
    
    std::string line;
    while (std::getline(std::cin, line))
    {
        try
        {
            interpreter.process(line);
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
}

