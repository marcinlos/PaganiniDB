#include "config.h"
//#include "Interpreter.h"
#include "DatabaseServer.h"
#include <paganini/database/DatabaseEngine.h>

using namespace paganini;

int main() 
{
    //paganini::server::DatabaseServer server;
    engine::DatabaseEngine engine("db", true);
    //server.run();
    return 0;
}
