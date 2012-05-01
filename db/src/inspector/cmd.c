#include "cmd.h"
#include <stdlib.h>
#include <string.h>

struct command* _head;

void register_cmd(const char* cmd, handler f, const char* desc)
{
    struct command* c = malloc(sizeof(struct command));
    strcpy(c->name, cmd);
    int len = strlen(desc);
    c->desc = malloc(len + 1);
    strcpy(c->desc, desc);
    c->f = f;
    c->next = _head;
    _head = c;
}

handler get_handler(const char* cmd)
{
    struct command* head = _head;
    while (head != NULL && strcmp(head->name, cmd) != 0)
    {
        head = head->next;
    }
    return head != NULL ? head->f : NULL;
}

struct command* get_commands(void)
{
    return _head;
}
