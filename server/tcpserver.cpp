
#include <boost/thread/thread.hpp>
#include "common/message.hpp"
#include "common/connection.hpp"
#include "tcpserver.hpp"
#include "spdlog/spdlog.h"

namespace sindia {

TcpServer::TcpServer(const std::string& ipaddr, uint32_t port)
	: m_work(m_service) 
	, m_endpoint(ip::address::from_string(ipaddr), port)
	, m_acceptor(m_service, m_endpoint)

{
}

void TcpServer::Start()
{
	spdlog::get("console")->info("server gets start");
	m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	m_acceptor.listen();
    Accept();
	m_runthread.reset(new std::thread([this]
    { 
        m_service.run(); 
    }));
}

void TcpServer::Accept()
{
	spdlog::get("console")->info("server accepting");
	auto new_connection = std::make_shared<Connection>(m_service);
	m_acceptor.async_accept(new_connection->Socket(), [this, new_connection](boost::system::error_code const& error)
	{
		if (!error)
		{
		    spdlog::get("console")->info("server accept new connection");
			std::string peeraddr = new_connection->GetPeerAddress();
            m_connmap[peeraddr] = new_connection;
			new_connection->SetCloseCallback(std::bind(&TcpServer::CloseConn, this, std::placeholders::_1));
			new_connection->Start();
		}
		else
		{
			spdlog::get("console")->info("server accept new connection error");
		}

		Accept();
	});
}

void TcpServer::Stop()
{
	spdlog::get("console")->info("server stop");

	m_acceptor.close();
    m_service.stop();

	for (auto& conn : m_connmap) {
		auto connptr = conn.second.lock();
		if (connptr)
	    {
		    connptr->Close();
		}
	}

    if (m_runthread->joinable())
	{
		m_runthread->join();
	}
}

void TcpServer::CloseConn(Connection_ptr conn)
{
	spdlog::get("console")->info("server close connection");

	std::unique_lock<std::mutex> lock(m_mutex);
	auto itr = m_connmap.find(conn->GetPeerAddress());
	if (itr != m_connmap.end())
    {
         m_connmap.erase(itr);
    }
}

}
