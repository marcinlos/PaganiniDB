#include "config.h"
#include "paging/page_manager.h"
#include "paging/dbfile_header.h"
#include "error_msg.h"
#include <stdio.h>
#include <time.h>

// Wypisywanie naglowka strony

static char* _str_page_number(char* buffer, pdbPageNumber number)
{
    if (number == NULL_PAGE)
        strcpy(buffer, "NULL_PAGE");
    else 
        sprintf(buffer, "%d", number);
    return buffer;
}

static char* _str_object_id(char* buffer, pdbObjectId id)
{
    if (id == NULL_OBJ)
        strcpy(buffer, "NULL_OBJ");
    else 
        sprintf(buffer, "%d", id);
    return buffer;
}

static char* _str_page_type(pdbPageFlags flags)
{
    pdbPageType type = PAGE_TYPE(flags);
    switch (type)
    {
    case UNUSED_PAGE: return "unused";
    case HEADER_PAGE: return "header";
    case DATA_PAGE: return "data";
    case UV_PAGE: return "usage vector";
    default: return "INVALID";
    }
}

static char* _str_flags(char* buffer, pdbPageFlags flags)
{
    sprintf(buffer, "%s", "-");
    return buffer;
}

static void _print_page_header(const pdbPageHeader* header)
{
    char buffer[64];
    const char* fmt = "%-15s %13s\n";
    
    _str_page_number(buffer, header->page_number);
    printf(fmt, "Page number", buffer);
    printf(fmt, "type", _str_page_type(header->flags));
    
    _str_page_number(buffer, header->prev);
    printf(fmt, "prev", buffer);
    
    _str_page_number(buffer, header->next);
    printf(fmt, "next", buffer);
    
    _str_object_id(buffer, header->owner);
    printf(fmt, "owner", buffer);
    
    printf("%-15s %13hd\n", "rows", header->rows);
    printf("%-15s %13hu\n", "free space", header->free_space);
    printf("%-15s %13hd\n", "free offset", header->free_offset);
    _str_flags(buffer, header->flags);
    printf(fmt, "flags", buffer);
    printf("\n");
}

void print_page_header(int argc, char** args)
{
    if (argc < 1)
    {
        error_usr("Za malo argumentow, oczekiwano liczby");
        return;
    }
    unsigned int page_number;
    if (sscanf(args[0], "%u", &page_number) == 0)
    {
        error_usr("Niepoprawny argument: '%s', oczekwano liczby", args[0]);
        return;
    }
    pdbPageBuffer buffer;
    pdbPageHeader* header = (pdbPageHeader*) buffer;
    if (pdbReadPage(page_number, buffer) < 0)
    {
        error_pdb("Nie udalo sie odczytac strony %d", page_number);
        return;
    }
    _print_page_header(header);    
}

// Wypisywanie naglowka bazy danych

void _print_db_header(const pdbDatabaseHeader* dbh)
{
    printf("%-15s %25s\n", "db name", dbh->db_name);
    printf("%-15s %25u\n", "page count", dbh->page_count);
    
    struct tm* t = gmtime(&(dbh->creation_time));
    char buffer[1024];
    strftime(buffer, 1024, "%H:%M:%S %d.%m.%G", t); 
    printf("%-15s %25s\n", "creation time", buffer);

    printf("\n");
}

void print_db_header(int argc, char** args)
{
    pdbPageBuffer buffer;
    pdbDatabaseHeader* dbh = (pdbDatabaseHeader*) (buffer + DATA_OFFSET);
    if (pdbReadPage(HEADER_PAGE_NUMBER, buffer) < 0)
    {
        error_pdb("Nie udalo sie odczytac naglowka pliku");
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

void _print_uv_content(pdbPageBuffer buffer)
{
    pdbData d = buffer + DATA_OFFSET;
    int i;
    for (i = 0; i < PAGES_PER_UV / 8; ++ i)
    {
        if (i % 8 == 0)
            putchar('\n');
        _print_byte(d[i]);
        putchar(' ');
    }
    putchar('\n');
}

void print_uv_content(int argc, char** args)
{
    if (argc < 1)
    {
        error_usr("Za malo argumentow, oczekiwano liczby");
        return;
    }
    unsigned int page_number;
    if (sscanf(args[0], "%u", &page_number) == 0)
    {
        error_usr("Niepoprawny argument: '%s', oczekwano liczby", args[0]);
        return;
    }
    if (! IS_UV(page_number))
    {
        error_usr("To nie jest strona UV");
        return;
    }
    pdbPageBuffer buffer;
    if (pdbReadPage(page_number, buffer) < 0)
    {
        error_pdb("Nie udalo sie odczytac strony %d", page_number);
        return;
    }
    _print_uv_content(buffer);
}



