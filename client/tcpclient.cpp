#include <boost/thread/thread.hpp>
#include "common/message.hpp"
#include "common/connection.hpp"
#include "tcpclient.hpp"
#include "spdlog/spdlog.h"

using namespace std;
using namespace boost::asio;

namespace sindia 
{
TcpClient::TcpClient(const std::string& ipaddr, uint32_t port)
	: m_service()
	, m_work(m_service)
	, m_endpoint(ip::address::from_string(ipaddr), port)
	, m_isconnected(false)
{
}

void TcpClient::Start()
{
	spdlog::get("console")->info("client gets start");
	StartConnect();

	m_runthread.reset(new std::thread([this]
    {   
        m_service.run();
    }));
}

void TcpClient::StartConnect()
{
	spdlog::get("console")->info("client start to connect");
    m_connection.reset(new Connection(m_service));
	m_connection->SetCloseCallback(std::bind(&TcpClient::CloseConn, this, std::placeholders::_1));
	m_connection->Socket().async_connect(m_endpoint, std::bind(&TcpClient::HandleConnection, this, std::placeholders::_1));
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
			    spdlog::get("console")->info("client try to reconnect");
				StartConnect();
			}
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	});
}

void TcpClient::HandleConnection(const boost::system::error_code& error)
{
	spdlog::get("console")->info("client handle connect");
	if (!m_connection->Socket().is_open())
    {
        spdlog::get("console")->info("connect failed, socket not open!");
		return;
	}

	if (!error)
	{
        spdlog::get("console")->info("client on connect");
		
		if (m_onconnect)
		{
		    spdlog::get("console")->info("client do connect handle");
			m_onconnect();
		}
		m_isconnected = true;
		m_connection->Start();

        char data[] = "hello server";
		Message msg(data, sizeof(data));
		msg.set_type(Message::NOTIFY);
		msg.set_id(0);
		msg.set_hash(0);
		msg.set_errcode(0);
		spdlog::get("console")->info("message info type(%u), length(%u), id(%u), hash(%u), error(%u), context %s",
			msg.get_type(), msg.data_length(),msg.get_id(),msg.get_hash(),msg.get_errcode(), msg.buffer());

		m_connection->Write(msg);
	}
	else
	{
        spdlog::get("console")->info("client on connect error, %s", (error.message()).c_str());

		if (m_onerror)
	    {
			m_onerror();
	    }
		
		CheckConnect();
	}
}

void TcpClient::CloseConn(Connection_ptr conn)
{
	spdlog::get("console")->info("client on close connect");

    if (!conn)
    {
        spdlog::get("console")->info("connection has close");
        return;
    }
	
	m_isconnected = false;
	m_connection.reset();
	CheckConnect();
}

}


