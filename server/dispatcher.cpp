#include "connection.hpp"
#include "dispatcher.hpp"
#include "rpcchannel.hpp"

namespace rpc 
{
void Dispatcher::reghandler(std::string& name,  Handler_t& handler)
{
    size_t hash = std::hash<std::string>{}(name);
	m_callmap.emplace(hash, handler);
}

void Dispatcher::handle(Message& msg, std::string& res)
{
	auto itr = m_callmap.find(msg.hash());
	if (itr == m_callmap.end())
	{
		return;
	}
	
	auto& handler = itr->second;
	if (!handler)
	{
		return;
	}
	
	std::string req(msg.body(), msg.bodylen());
	handler(req, res);
}

void Dispatcher::dispatch(Connection_ptr conn, Message& msg)
{
    if (msg.type() == Message::RESPONSE)
	{
	    rpcchnl.SetResponse(msg);    
	}
    else if(msg.type() == Message::REQUEST)
    {
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
        std::string res;
        handle(msg, res);
	}
	else
	{
	    //error log
	}
}
}
