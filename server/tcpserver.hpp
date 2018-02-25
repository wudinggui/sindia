#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include <string>
#include <thread>
#include <functional>
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <boost/asio.hpp>
#include "common/connection.hpp"

using namespace std;
using namespace boost::asio;

namespace sindia
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

	void Accept();
	void CloseConn(Connection_ptr conn);

    io_service 								m_service;
	io_service::work                        m_work;
    ip::tcp::endpoint   					m_endpoint;
    ip::tcp::acceptor 						m_acceptor;
	std::shared_ptr<std::thread>            m_runthread;
	std::mutex			                    m_mutex;
    std::unordered_map<std::string, Connection_weak>  m_connmap;	
};
}
#endif /// TCP_SERVER_H__

