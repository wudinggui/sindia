#include <boost/thread/thread.hpp>
#include "message.hpp"
#include "connection.hpp"
#include "tcpclient.hpp"

using namespace std;
using namespace boost::asio;

namespace rpc 
{
TcpClient::TcpClient(const std::string& ipaddr, uint32_t port)
	: m_service()
	, m_work(m_service)
	, m_socket(m_service)
	, m_endpoint(ip::address::from_string(ipaddr), port)
	, m_isconnected(false)
	, m_connection(std::make_shared<Connection>(m_service))
{
}

void TcpClient::Run()
{
	m_service.run();
}

void TcpClient::Start()
{
	m_runthread.reset(new std::thread(std::bind(&TcpClient::Run, this)));
	StartConnect();
}

ip::tcp::socket& TcpClient::Socket()
{
	return m_socket;
}

ip::tcp::endpoint const& TcpClient::Endpoint() const
{
	return m_endpoint;
}

void TcpClient::StartConnect()
{
    boost::system::error_code error;
	m_socket.async_connect(m_endpoint, std::bind(&TcpClient::HandleConnection, this, error));
}

void TcpClient::CheckConnect()
{
	if (m_chkthread != nullptr)
	{
		return;
	}
	
	m_chkthread = std::make_shared<std::thread>([this]
	{
		while (true)
		{
			if (!m_isconnected)
			{
				StartConnect();
			}
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	});
}

void TcpClient::HandleConnection(const boost::system::error_code& error)
{
	if (!error)
	{
		if (m_onconnect)
		{
			m_onconnect();
		}
		m_isconnected = true;
		m_connection->SetCloseCallback(std::bind(&TcpClient::CloseConn, this, std::placeholders::_1));
		m_connection->Start();
	}
	else
	{
		if (m_onerror)
	    {
			m_onerror();
	    }

		CheckConnect();
	}
}

void TcpClient::CloseConn(Connection_ptr conn)
{
    if (!conn)
    {
        return;
    }
	
	m_isconnected = false;
	//std::cout << error.message() << std::endl;
	m_connection.reset();
	m_connection = std::make_shared<Connection>(m_service);
	CheckConnect();
}

}


