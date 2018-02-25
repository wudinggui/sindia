#ifndef TCP_CONNECTION_H__
#define TCP_CONNECTION_H__

#include <string>
#include <string>
#include <thread>
#include <mutex>
#include <functional>
#include <list>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "message.hpp"

using namespace std;
using namespace boost::asio;


namespace sindia
{
class Connection;
using Connection_weak = std::weak_ptr<Connection>;
using Connection_ptr = std::shared_ptr<Connection>;
using CloseCallback = std::function<void (const Connection_ptr&)>;

class Connection : public std::enable_shared_from_this<Connection>
{
public:
	Connection(io_service& ios);
	~Connection(){std::cout<<"~Connection"<<std::endl;};

    void Start();
	void Stop();
	void Write(Message& msg);
    ip::tcp::socket& Socket();
	std::string GetPeerAddress();
	void Close();
	void SetCloseCallback(const CloseCallback& cb);

private:
	void ReadHead();
	void ReadBody();
    void OnWrite(const boost::system::error_code& error);
	void OnError(const boost::system::error_code& error);
	void OnReadHead(const boost::system::error_code& error);
	void OnReadBody(const boost::system::error_code& error);

	ip::tcp::socket						    m_socket;
	std::string                             m_peeraddr;
    Message                                 m_message;
	mutable std::mutex                      m_mutex;
	std::list<Message>                      m_sendmsgqueue;
	CloseCallback                           m_closecb;
};
}

#endif
