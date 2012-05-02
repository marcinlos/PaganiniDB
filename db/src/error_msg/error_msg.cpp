#include "config.h"
#include "pdb_error.h"
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstdarg>
#include <cstring>

using paganini::Error;
using paganini::pdbErrno;
using paganini::pdbErrorMsg;

const static int _MAX_MSG_LEN = 1024;

static void _error_core(int error, const char* text, va_list args)
{
    char buffer[_MAX_MSG_LEN];
    vsprintf(buffer, text, args);
    if (error != 0)
        sprintf(buffer + strlen(buffer), "\nPowod: %s", strerror(error));
        
    strcat(buffer, "\n");
    fputs(buffer, stderr);
    fflush(nullptr);
}

void fatal_sys(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    _error_core(errno, text, args);
    va_end(args);
    exit(-1);
}

void error_sys(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    _error_core(errno, text, args);
    va_end(args);
}

void fatal_usr(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    _error_core(0, text, args);
    va_end(args);
    exit(-1);
}

void error_usr(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    _error_core(0, text, args);
    va_end(args);
}


static void _pdb_error_core(Error error, const char* text, va_list args)
{
    char buffer[_MAX_MSG_LEN];
    vsprintf(buffer, text, args);
    if (error != Error::NONE)
        sprintf(buffer + strlen(buffer), "\nPowod: %s", pdbErrorMsg(error));
        
    strcat(buffer, "\n");
    fputs(buffer, stderr);
    fflush(nullptr);
}

void fatal_pdb(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    _pdb_error_core(pdbErrno(), text, args);
    va_end(args);
    exit(-1);
}

void error_pdb(const char* text, ...)
{
    va_list args;
    va_start(args, text);
    _pdb_error_core(pdbErrno(), text, args);
    va_end(args);
}
