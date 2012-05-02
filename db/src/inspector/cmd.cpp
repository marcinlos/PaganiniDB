#include "cmd.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
using std::vector;


void CommandExecutor::registerCmd(const string& name, Handler f, 
    const string& desc)
{
    Command cmd(f, desc);
    commands[name] = cmd;
}

void CommandExecutor::operator ()(const string& command)
{
    vector<string> args;
    static const char delims[] = "\t ";
    vector<char> buffer(command.begin(), command.end());
    
    const char* str = strtok(&buffer[0], delims);
    if (str == nullptr)
        return;
        
    auto i = commands.find(str);
    str = strtok(nullptr, delims);
    
    if (i != commands.end())
    {
        Handler f = i->second.f;
        while (str != nullptr)
        {
            args.push_back(str);
            str = strtok(nullptr, delims);
        }
        f(args);
    } 
}


