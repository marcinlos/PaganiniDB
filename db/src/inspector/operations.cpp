#include "config.h"
#include <paganini/paging/PageManager.h>
#include <paganini/paging/FilePersistenceManager.h>
#include <paganini/paging/DummyLocker.h>
#include <paganini/paging/DatabaseHeader.h>
#include <paganini/error_msg.h>
#include <paganini/Error.h>
#include "operations.h"
#include <cstdio>
#include <ctime>
#include <cstring>

using namespace paganini;

extern PageManager<FilePersistenceManager<DummyLocker>>* manager;

// Wypisywanie naglowka strony

static char* _str_page_number(char* buffer, page_number number)
{
    if (number == NULL_PAGE)
        strcpy(buffer, "NULL_PAGE");
    else 
        sprintf(buffer, "%d", number);
    return buffer;
}

static char* _str_object_id(char* buffer, object_id id)
{
    if (id == NULL_OBJ)
        strcpy(buffer, "NULL_OBJ");
    else 
        sprintf(buffer, "%d", id);
    return buffer;
}

static const char* _str_page_type(page_flags flags)
{
    PageType type = get_page_type(flags);
    switch (type)
    {
    case PageType::UNUSED: return "unused";
    case PageType::HEADER: return "header";
    case PageType::DATA: return "data";
    case PageType::UV: return "usage vector";
    default: return "INVALID";
    }
}

static char* _str_flags(char* buffer, page_flags flags)
{
    sprintf(buffer, "%s", "-");
    return buffer;
}

static void _print_page_header(const PageHeader& header)
{
    char buffer[64];
    const char* fmt = "%-15s %13s\n";
    
    _str_page_number(buffer, header.number);
    printf(fmt, "Page number", buffer);
    printf(fmt, "type", _str_page_type(header.flags));
    
    _str_page_number(buffer, header.prev);
    printf(fmt, "prev", buffer);
    
    _str_page_number(buffer, header.next);
    printf(fmt, "next", buffer);
    
    _str_object_id(buffer, header.owner);
    printf(fmt, "owner", buffer);
    
    printf("%-15s %13hd\n", "rows", header.rows);
    printf("%-15s %13hu\n", "free space", header.free_space);
    printf("%-15s %13hd\n", "free offset", header.free_offset);
    _str_flags(buffer, header.flags);
    printf(fmt, "flags", buffer);
    printf("\n");
}

void print_page_header(const vector<string>& args)
{
    if (args.size() < 1)
    {
        error_usr("Za malo argumentow, oczekiwano liczby");
        return;
    }
    unsigned int page_number;
    if (sscanf(args[0].c_str(), "%u", &page_number) == 0)
    {
        error_usr("Niepoprawny argument: '%s', oczekwano liczby", 
            args[0].c_str());
        return;
    }
    PageBuffer page;
    try { manager->readPage(page_number, &page); }
    catch (Exception& e)
    {
        printf("%s\n%s\n", e.what(), e.getCodeMessage());
        error_usr("Nie udalo sie odczytac strony %d", page_number);
        return;
    }
    _print_page_header(page.header);    
}

// Wypisywanie naglowka bazy danych

void _print_db_header(const DatabaseHeader* dbh)
{
    printf("%-15s %25s\n", "db name", dbh->db_name);
    printf("%-15s %25u\n", "page count", dbh->page_count);
    
    struct tm* t = gmtime(&(dbh->creation_time));
    char buffer[1024];
    strftime(buffer, 1024, "%H:%M:%S %d.%m.%G", t); 
    printf("%-15s %25s\n", "creation time", buffer);

    printf("\n");
}

void print_db_header(const vector<string>& args)
{
    PageBuffer page;
    DatabaseHeader* dbh = page.get<DatabaseHeader>();
    try { manager->readPage(HEADER_PAGE_NUMBER, &page); }
    catch (Exception& e)
    {
        printf("%s\n%s\n", e.what(), e.getCodeMessage());
        error_usr("Nie udalo sie odczytac naglowka pliku");
        return;
    }
    _print_db_header(dbh);   
}

// Wypisywanie zawartosci strony UV

void _print_byte(unsigned char b)
{
    int mask = 1;
    while (mask < (1 << 8))
    {
        putchar(b & mask ? '$' : '.'); 
        mask <<= 1;
    }
}

void _print_uv_content(const PageBuffer* page)
{
    for (unsigned int i = 0; i < PAGES_PER_UV / 8; ++ i)
    {
        if (i % 8 == 0)
            putchar('\n');
        _print_byte(page->data[i]);
        putchar(' ');
    }
    putchar('\n');
}

void print_uv_content(const vector<string>& args)
{
    if (args.size() < 1)
    {
        error_usr("Za malo argumentow, oczekiwano liczby");
        return;
    }
    unsigned int page_number;
    if (sscanf(args[0].c_str(), "%u", &page_number) == 0)
    {
        error_usr("Niepoprawny argument: '%s', oczekwano liczby", 
            args[0].c_str());
        return;
    }
    if (! isUV(page_number))
    {
        error_usr("To nie jest strona UV");
        return;
    }
    PageBuffer page;
    try { manager->readPage(page_number, &page); }
    catch (Exception& e)
    {
        printf("%s\n%s\n", e.what(), e.getCodeMessage());
        error_usr("Nie udalo sie odczytac strony %d", page_number);
        return;
    }
    _print_uv_content(&page);
}



