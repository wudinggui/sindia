#include <boost/bind.hpp>
#include "connection.hpp"

namespace rpc
{
Connection::Connection(io_service& ios)
	: m_socket(ios)
{
}

void Connection::SetCloseCallback(const CloseCallback& cb)
{
    m_closecb = cb;
}

void Connection::Close()
{
	boost::system::error_code error;
	//m_socket.shutdown(tcp::socket::shutdown_both, error);
	m_socket.close(error);

	if (m_closecb) {
		m_closecb(shared_from_this());
	}
}

ip::tcp::socket& Connection::Socket()
{
	return m_socket;
}

std::string Connection::GetPeerAddress()
{
    std::string peerip;
    std::string peerport;
	std::string peeraddr;

    assert(m_socket.is_open());
	std::stringstream ss;
    ss << m_socket.remote_endpoint().port();
    ss >> peerport;
    ss << m_socket.remote_endpoint().address().to_string();
    ss >> peerip;

    peeraddr = peerip + ":" + peerport;
	return peeraddr;
}

void Connection::Start()
{
	ReadHead();
}

void Connection::Stop()
{
	Close();
}

void Connection::OnReadHead(boost::system::error_code const& error)
{
	if (!error)
	{
		if (!m_readmsg.decode_msg())
		{
			ReadHead();
		}
		else
		{
			ReadBody();
		}
	}
	else
	{
		OnError(error);
	}
}

void Connection::OnReadBody(boost::system::error_code const& error)
{
	auto self = this->shared_from_this();
	if (!error)
	{
	    //todo callback here, like cb(self, m_readmsg);
		ReadHead();
	}
	else
	{
		OnError(error);
	}
}

void Connection::OnError(boost::system::error_code const& error)
{
	if (!error)
	{
        return;
	}

	Close();
}

void Connection::ReadHead()
{
	boost::system::error_code  error;
	auto self = this->shared_from_this();

	async_read(m_socket,
		      boost::asio::buffer(m_readmsg.data(), m_readmsg.headerlen()),
		      boost::bind(&Connection::OnReadHead, self, error));
}

void Connection::ReadBody()
{
	boost::system::error_code  error;
	auto self = this->shared_from_this();

	async_read(m_socket,
		       boost::asio::buffer(m_readmsg.body(), m_readmsg.bodylen()),
		       boost::bind(&Connection::OnReadBody, self, error));
}

void Connection::Write(Message& msg)
{
	boost::system::error_code  error;
	std::unique_lock<std::mutex> lock(m_mutex);

	if (m_sendmsgqueue.empty())
	{
		async_write(m_socket,
			        boost::asio::buffer(msg.data(), msg.length()),
			        boost::bind(&Connection::OnWrite, this, error));
	}
	else
	{
		m_sendmsgqueue.emplace_back(msg);
	}
}

void Connection::OnWrite(boost::system::error_code const& error)
{
	if (!error)
	{
		if (!m_sendmsgqueue.empty())
		{
            auto msg = m_sendmsgqueue.front();
			m_sendmsgqueue.pop_front();
			async_write(m_socket,
						boost::asio::buffer(msg.data(), msg.length()),
						boost::bind(&Connection::OnWrite, this, error));
		}
	}
	else
	{
		OnError(error);
		if (!m_sendmsgqueue.empty())
	    {
            m_sendmsgqueue.clear();
	    }
	}
}
}

