#include "config.h"
#include <paganini/error_msg.h>
#include <paganini/Error.h>
#include <paganini/paging/PageManager.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/paging/DummyLocker.h>
#include "operations.h"
#include "cmd.h"
#include <iostream>
using namespace paganini;

CommandExecutor executor;
PageManager<FilePersistenceManager<DummyLocker>>* manager;

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
    try
    {
        // PageManager& manager = PageManager::getInstance();
        manager = new PageManager<FilePersistenceManager<DummyLocker>>;
        manager->openFile(argv[1]);
        
        prepare();
        std::cout << "Uzyj 'help' by zobaczyc liste komend" << std::endl;
        input_loop();
        
        manager->closeFile();
    }
    catch (paganini::Exception& e)
    {
        std::cout << "Fatal error: " << e.what() << std::endl
            << e.getCodeMessage() << std::endl;
    }
    return 0;
}
