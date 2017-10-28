#include "tcpserver.hpp"

using namespace rpc;

int main()
{
    TcpServer  server("127.0.0.1", 4000);
	server.Start();

	while(1)
	{
		(void)sleep(1);
	}
	
	return 0;
}
