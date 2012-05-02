#include "config.h"
#include "error_msg.h"
#include "paging/page_manager.h"
#include "operations.h"
#include "cmd.h"
#include <iostream>
using namespace paganini;

CommandExecutor executor;


void input_loop()
{
    std::cout << "> ";
    string line;
    while (std::getline(std::cin, line))
    {
        executor(line);
        std::cout << "> ";
    }
}

void prepare()
{
    executor.registerCmd("ph", print_page_header, "Wypisuje naglowek strony");
    executor.registerCmd("dbh", print_db_header, "Wypisuje naglowek pliku");
    executor.registerCmd("uv", print_uv_content, "Wypisuje zawartosc strony UV");
}

int main(int argc, char** argv)
{
    if (argc != 2)
        fatal_usr("Uzycie: insp nazwa_pliku");

    pdbPageManagerStart("dupa");
    
    prepare();
    std::cout << "Uzyj 'help' by zobaczyc liste komend" << std::endl;
    input_loop();
    
    pdbPageManagerStop();
    return 0;
}
