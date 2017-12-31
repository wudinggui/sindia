#include "connection.hpp"
#include "dispatcher.hpp"
#include "rpcchannel.hpp"
#include "spdlog/spdlog.h"

namespace rpc 
{
void Dispatcher::reghandler(std::string& name,  Handler_t& handler)
{
	spdlog::get("console")->info("dispatcher reg handle %s", name.c_str());
    size_t hash = std::hash<std::string>{}(name);
	m_callmap.emplace(hash, handler);
}

void Dispatcher::handle(Message& msg, std::string& res)
{
	spdlog::get("console")->info("dispatcher get handle hash id %d", msg.hash());

	auto itr = m_callmap.find(msg.hash());
	if (itr == m_callmap.end())
	{
	    spdlog::get("console")->info("dispatcher get handle fail");
		return;
	}
	
	auto& handler = itr->second;
	if (!handler)
	{
	    spdlog::get("console")->info("dispatcher get handle null");
		return;
	}
	
	std::string req(msg.body(), msg.bodylen());
	handler(req, res);
}

void Dispatcher::dispatch(Connection_ptr conn, Message& msg)
{
    if (msg.type() == Message::RESPONSE)
	{
		spdlog::get("console")->info("dispatch response msg");

	    rpcchnl.SetResponse(msg);    
	}
    else if(msg.type() == Message::REQUEST)
    {
		spdlog::get("console")->info("dispatch resquest msg");

        std::string res;
        handle(msg, res);
		
        Message::Header newhead(msg.header());
		newhead.type = Message::RESPONSE;
		newhead.length = res.size();
        msg.encode_msg(newhead, (char*)res.c_str(), res.size());
		if (conn)
		{
		    conn->Write(msg);
		}
    }
	else if(msg.type() == Message::NOTIFY)
	{
		spdlog::get("console")->info("dispatch notify msg");

        std::string res;
        handle(msg, res);
	}
	else
	{
	    spdlog::get("console")->info("dispatch error msg");
	}
}
}
