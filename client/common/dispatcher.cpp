#include "connection.hpp"
#include "dispatcher.hpp"
#include "rpcchannel.hpp"
#include "spdlog/spdlog.h"

namespace sindia 
{
void Dispatcher::reghandler(std::string name, const Handler_t& handler)
{
    uint32_t hash = std::hash<std::string>{}(name);
	m_callmap.emplace(hash, handler);
	spdlog::get("console")->info("dispatcher reg handle %s, hast %u", name.c_str(), hash);
}

void Dispatcher::handle(Message& msg, std::string& res)
{
	spdlog::get("console")->info("dispatcher get handle hash id %d", msg.get_hash());

	auto itr = m_callmap.find(msg.get_hash());
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
	
	std::string req(msg.data(), msg.data_length());
	res = handler(req);
}

void Dispatcher::dispatch(Connection_ptr conn, Message& msg)
{
    if (msg.get_type() == Message::RESPONSE)
	{
		spdlog::get("console")->info("dispatch response msg");

	    rpcchnl.SetResponse(msg);    
	}
    else if(msg.get_type() == Message::REQUEST)
    {
		spdlog::get("console")->info("dispatch resquest msg");

        std::string res;
        handle(msg, res);
		
		Message resp(res.c_str(), res.size());
		resp.set_type(Message::RESPONSE);
		resp.set_hash(msg.get_hash());
		resp.set_id(msg.get_id());
		resp.set_errcode(0);
		
		if (conn)
		{
		    conn->Write(resp);
		}
    }
	else if(msg.get_type() == Message::NOTIFY)
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
