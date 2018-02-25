#include "tcpserver.hpp"
#include "spdlog/spdlog.h"

using namespace sindia;

int main()
{
    //log init
	(void)spdlog::stdout_logger_mt("console");
	
	spdlog::get("console")->info("main starts 127.0.0.1");

    TcpServer  server("127.0.0.1", 6666);
	server.Start();

	while(1)
	{
		(void)sleep(1);
	}
	
	return 0;
}
