#ifndef TCP_DISPATCHER_H__
#define TCP_DISPATCHER_H__

#include <string>
#include <thread>
#include <functional>
#include <unordered_map>
#include <future>
#include <boost/asio.hpp>
#include "message.hpp"

using namespace std;
using namespace boost::asio;

namespace rpc 
{
class Dispatcher
{
    using Handler_t = std::function<void(std::string& req, std::string& res)>;
    using Callmap_t = std::unordered_map<size_t, Handler_t>;
public:
    Dispatcher(){};
    void reghandler(std::string& name,  Handler_t& handler);
    void dispatch(Connection_ptr conn, Message& msg);
    void handle(Message& msg, std::string& res);

private:
    Callmap_t                       m_callmap;
};
}

#endif
