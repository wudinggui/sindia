#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include "connection.hpp"
#include "spdlog/spdlog.h"

using namespace boost::asio;

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
    spdlog::get("console")->info("connection close");

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
    std::string peeraddr;

    assert(m_socket.is_open());

    peerip = m_socket.remote_endpoint().address().to_string();
    peeraddr = peerip + ":" 
             + boost::lexical_cast<std::string>(m_socket.remote_endpoint().port());
    
    spdlog::get("console")->info("connection get peeraddr %s", peeraddr.c_str());
        
    return peeraddr;
}

void Connection::Start()
{
    spdlog::get("console")->info("connection get start");
    ReadHead();
}

void Connection::Stop()
{
    spdlog::get("console")->info("connection get stop");
    Close();
}

void Connection::OnReadHead(const boost::system::error_code& error)
{
    if (!error)
    {
        spdlog::get("console")->info("connection on read msg header");
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
        spdlog::get("console")->info("connection on read msg header error");
        OnError(error);
    }
}

void Connection::OnReadBody(const boost::system::error_code& error)
{
    auto self = this->shared_from_this();
    if (!error)
    {
        //spdlog::get("console")->info("connection on read msg body");
        //todo callback here, like cb(self, m_readmsg);
        ReadHead();
    }
    else
    {
        spdlog::get("console")->info("connection on read msg body error");
        OnError(error);
    }
}

void Connection::OnError(const boost::system::error_code& error)
{
    if (!error)
    {
        return;
    }

    spdlog::get("console")->info("connection on error");

    Close();
}

void Connection::ReadHead()
{
    auto self = this->shared_from_this();

    spdlog::get("console")->info("connection start async read head");

    async_read(m_socket,
              boost::asio::buffer(m_readmsg.data(), m_readmsg.headerlen()),
              boost::bind(&Connection::OnReadHead, self, boost::asio::placeholders::error));
}

void Connection::ReadBody()
{
    auto self = this->shared_from_this();

    //spdlog::get("console")->info("connection start async read body");

    async_read(m_socket,
               boost::asio::buffer(m_readmsg.body(), m_readmsg.bodylen()),
               boost::bind(&Connection::OnReadBody, self, boost::asio::placeholders::error));
}

void Connection::Write(Message& msg)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_sendmsgqueue.empty())
    {
        spdlog::get("console")->info("connection start async write at once");

        async_write(m_socket,
                    boost::asio::buffer(msg.data(), msg.length()),
                    boost::bind(&Connection::OnWrite, this, boost::asio::placeholders::error));
    }
    else
    {
        spdlog::get("console")->info("connection start async write later");

        m_sendmsgqueue.emplace_back(msg);
    }
}

void Connection::OnWrite(const boost::system::error_code& error)
{
    if (!error)
    {
        if (!m_sendmsgqueue.empty())
        {
            spdlog::get("console")->info("connection on write and write pending write msg");

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
            spdlog::get("console")->info("connection on write error and clear pending msg");
            m_sendmsgqueue.clear();
        }
    }
}
}

