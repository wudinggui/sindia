#ifndef TCP_CLIENT_H__
#define TCP_CLIENT_H__

#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>
#include "common/connection.hpp"

using namespace std;
using namespace boost::asio;

namespace sindia 
{
class Connection;
class TcpClient
{
public:
	TcpClient(const std::string& ipaddr, uint32_t port);
	~TcpClient(){std::cout << "~TcpClient" << std::endl;};
	void Start();
private:
	void StartConnect();
	void HandleConnection(const boost::system::error_code& error);
	void CheckConnect();
	void CloseConn(Connection_ptr conn);

private:
    io_service 					  m_service;
	io_service::work              m_work;
	ip::tcp::endpoint			  m_endpoint;
	std::atomic<bool>             m_isconnected;         

	std::function<void()>		  m_onconnect;
	std::function<void()>		  m_onerror;
	Connection_ptr                m_connection;
	std::shared_ptr<std::thread>  m_chkthread;
	std::shared_ptr<std::thread>  m_runthread;
};
}

#endif
