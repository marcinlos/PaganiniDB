#ifndef _CMD_H
#define _CMD_H

#include <string>
#include <vector>
#include <map>
#include <functional>
using std::string;
using std::vector;
using std::map;


typedef std::function<void (const vector<string>&)> Handler;

struct Command
{
    Handler f;
    string desc;
    
    Command() { }
    
    Command(Handler f, const string& desc): f(f), desc(desc)
    {
    }
};


class CommandExecutor
{
private:
    map<string, Command> commands;
    
public:
    void registerCmd(const string& name, Handler f, const string& desc);
    void operator ()(const string& command);
};

#endif /* _CMD_H */

