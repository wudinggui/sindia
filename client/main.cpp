#include "tcpclient.hpp"

using namespace rpc;

int main()
{
    TcpClient  client("127.0.0.1", 4000);
	client.Start();

	while(1)
	{
		(void)sleep(1);
	}
	
	return 0;
}
