
#include <boost/thread/thread.hpp>
#include "message.hpp"
#include "connection.hpp"
#include "tcpserver.hpp"

namespace rpc {

TcpServer::TcpServer(const std::string& ipaddr, uint32_t port)
	: m_socket(m_service)
	, m_endpoint(ip::address::from_string(ipaddr), port)
	, m_acceptor(m_service, m_endpoint)

{
}

void TcpServer::Run()
{
	m_service.run();
}

void TcpServer::Start()
{
    Accept();
	m_runthread.reset(new std::thread(std::bind(&TcpServer::Run, this)));
}

void TcpServer::Stop()
{
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

void TcpServer::Accept()
{
	auto new_connection = std::make_shared<Connection>(m_service);

	m_acceptor.async_accept(new_connection->Socket(), [this, new_connection](boost::system::error_code const&error)
	{
		if (!error)
		{
			std::string peeraddr = new_connection->GetPeerAddress();
            m_connmap[peeraddr] = new_connection;
			new_connection->SetCloseCallback(std::bind(&TcpServer::CloseConn, this, std::placeholders::_1));
			new_connection->Start();
		}
		else
		{
			// TODO log error
		}

		Accept();
	});
}

void TcpServer::CloseConn(Connection_ptr conn)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	for (auto itr = m_connmap.begin(); itr != m_connmap.end(); ++itr)
	{
		if (itr->second.lock() == conn)
		{
			itr = m_connmap.erase(itr);
		}
	}
}

}
