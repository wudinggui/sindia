#ifndef TCP_DISPATCHER_H__
#define TCP_DISPATCHER_H__

#include <string>
#include <thread>
#include <functional>
#include <unordered_map>
#include <future>
#include <boost/asio.hpp>
#include "message.hpp"
#include "itypedef.hpp"

using namespace std;
using namespace boost::asio;

namespace sindia 
{
class Dispatcher
{
public:
	Dispatcher(){};
	void reghandler(std::string name, const Handler_t& handler);
    void dispatch(Connection_ptr conn, Message& msg);
private:
	void handle(Message& msg, std::string& res);	
	Callmap_t                       m_callmap;
};
}

#endif
