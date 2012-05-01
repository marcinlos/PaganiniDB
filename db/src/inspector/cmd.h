#ifndef _CMD_H
#define _CMD_H

#define MAX_CMD_LEN 20

typedef void (*handler)(int, char**);

struct command
{
    char name[MAX_CMD_LEN];
    char* desc;
    handler f;
    struct command* next;
};

/* Dodaje komende o podanej nazwie */
void register_cmd(const char* cmd, handler f, const char* desc);

/* Zwraca funkcje implementujaca podana komende */
handler get_handler(const char* cmd);

/* Zwraca glowe listy zarejestrowanych komend */
struct command* get_commands(void);

#endif /* _CMD_H */

