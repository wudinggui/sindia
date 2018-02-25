#include "rpcchannel.hpp"
#include "spdlog/spdlog.h"

namespace sindia 
{

Rpcchannel::Rpcchannel()
	:m_time(DEFAULTTIMEOUT)
	,m_callid(0) 
{
}

void Rpcchannel::SetConnection(const Connection_ptr& conn)
{
    m_conn = conn;
}

int32_t Rpcchannel::Call(std::string& name,  std::string& req, std::string& res)
{
    if (!m_conn)
    {
        spdlog::get("console")->info("call function %s fail, connection not found", name.c_str());
        return RPCNOCONN;
    }
	
	spdlog::get("console")->info("call function %s", name.c_str());

	Message  msg(req.c_str(), req.size());
	msg.set_type(Message::REQUEST);
	msg.set_id(++m_callid);
	msg.set_hash(std::hash<std::string>{}(name));
	msg.set_errcode(0);

    m_conn->Write(msg);

    return GetResponse(m_callid, res);
}

int32_t Rpcchannel::GetResponse(uint32_t callid, std::string& res)
{
    std::promise<std::string> prom;
    std::future<std::string>  fut = prom.get_future(); //future¹ØÁª.
	m_prommap[callid] = std::move(prom);
	
	std::chrono::milliseconds timeout(m_time);
    while (fut.wait_for(timeout) == std::future_status::timeout)
    {
        spdlog::get("console")->info("get response fail, msg id %u, timeout", callid);
        return RPCTIMEOUT;
    }

    res = fut.get();

    spdlog::get("console")->info("get response msg, id %u", callid);

    auto itr = m_prommap.find(callid);
	if (itr != m_prommap.end())
	{
	    m_prommap.erase(itr);
	}
	
	return RPCOK;
}

void Rpcchannel::SetTimeout(uint32_t time)
{
    m_time = time;
}

void Rpcchannel::SetResponse(Message& msg)
{
    auto itr = m_prommap.find(msg.get_id());
	if (itr == m_prommap.end())
	{
	    spdlog::get("console")->info("set response msg fail, id %u", msg.get_id());
	    return;
	}

	auto& prom = itr->second;

	prom.set_value(std::string(msg.data(), msg.data_length()));
}

}

