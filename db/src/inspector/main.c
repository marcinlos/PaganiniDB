#include "error_msg.h"
#include "operations.h"
#include "cmd.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE    255
#define MAX_ARGS    10



static int tokenize(char* line, char** args, int max)
{
    static const char delims[] = "\t ";
    char* str = strtok(line, delims);
    int i = 0;
    
    while (str != NULL)
    {
        if (i < max)
            args[i ++] = str;
        else return -1;
        str = strtok(NULL, delims);
    }
    args[i] = NULL;
    return i;   
}

static void process_line(char* line)
{
    char* args[MAX_ARGS];
    int count = tokenize(line, args, MAX_ARGS - 1);
    if (count == -1)
        error_usr("Zbyt duzo argumentow (max %d)", MAX_ARGS);
    else if (count > 0)
    {
        handler f = get_handler(args[0]);
        if (f != NULL)
            f(count - 1, args + 1);
        else
            error_usr("Nieznana komenda: '%s'", args[0]);
    }
}

static void input_loop(void)
{
    char buffer[MAX_LINE], *nl;
    printf("> ");
    while (fgets(buffer, MAX_LINE, stdin) != NULL)
    {
        if ((nl = strchr(buffer, '\n')) != NULL)
            *nl = '\0';
        process_line(buffer);
        printf("> ");
    }
}

static void help(int argc, char** args)
{
    struct command* cmd = get_commands();
    while (cmd != NULL)
    {
        printf("  %-10s %-50s\n", cmd->name, cmd->desc);
        cmd = cmd->next;
    }
}

static void prepare()
{
    register_cmd("help", &help, "Wypisuje dostepne komendy");
    register_cmd("ph", &print_page_header, "Wypisuje naglowek strony");
    register_cmd("dbh", &print_db_header, "Wypisuje naglowek pliku");
    register_cmd("uv", &print_uv_content, "Wypisuje zawartosc strony UV");
}

int main(int argc, char** argv)
{
    if (argc != 2)
        fatal_usr("Uzycie: insp nazwa_pliku");

    pdbPageManagerStart("dupa");
    
    prepare();
    printf("Uzyj 'help' by zobaczyc liste komend\n");
    input_loop();
    
    pdbPageManagerStop();
    return 0;
}
