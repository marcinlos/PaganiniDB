//#include <paganini/tools/dbshell/Interpreter.h>
#include "Interpreter.h"
#include <string>
#include <iostream>


int main()
{
    paganini::shell::Interpreter interpreter;
    
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
