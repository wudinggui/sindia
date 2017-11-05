#include "tcpserver.hpp"
#include "spdlog/spdlog.h"

using namespace rpc;

int main()
{
    //log init
    (void)spdlog::stdout_logger_mt("console");
    
    spdlog::get("console")->info("main starts");

    TcpServer  server("127.0.0.1", 4000);
    server.Start();

    while(1)
    {
        (void)sleep(1);
    }
    
    return 0;
}
