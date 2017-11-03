#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include <string>
#include <thread>
#include <functional>
#include <boost/asio.hpp>
#include "common/connection.hpp"

using namespace std;
using namespace boost::asio;

namespace rpc
{
class Connection;
class TcpServer
{
public:
	TcpServer(const std::string& ipaddr, uint32_t port);
	~TcpServer(){};

	void Start();
	void Stop();

private:
	void Run();
	void Accept();
	void CloseConn(Connection_ptr conn);

    io_service 								m_service;
    ip::tcp::socket   						m_socket;
    ip::tcp::endpoint   					m_endpoint;
    ip::tcp::acceptor 						m_acceptor;
    std::map<std::string, Connection_weak>  m_connmap;
	std::shared_ptr<std::thread>            m_runthread;
	mutable std::mutex			            m_mutex;
};
}
#endif /// TCP_SERVER_H__

