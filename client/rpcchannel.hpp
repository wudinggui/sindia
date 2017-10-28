#ifndef RPC_CHANNEL_H__
#define RPC_CHANNEL_H__

#include <map>
#include <unordered_map>
#include <future>
#include <boost/serialization/singleton.hpp>

#include "message.hpp"
#include "connection.hpp"

namespace rpc 
{
static const uint32_t DEFAULTTIMEOUT = 1000u; //1000ms

class Rpcchannel
{
public:
using  Retprommap_t = std::unordered_map<uint32_t, std::promise<std::string>>;

typedef enum 
	{
		RPCOK = 0, 
		RPCFAIL, 
		RPCNOCONN, 
		RPCTIMEOUT,
	} RPC_RET;

	Rpcchannel();
	~Rpcchannel(){};
	void   SetTimeout(uint32_t value);
	void   SetResponse(Message& msg);
	void   SetConnection(const Connection_ptr& conn);

	int32_t   Call(std::string& name,  std::string& req, std::string& res);
	int32_t   GetResponse(uint32_t callid, std::string& res);

private:
	uint32_t               m_time;
	std::atomic<uint32_t>  m_callid; 
	Retprommap_t           m_prommap;
	Connection_ptr         m_conn;
};

typedef boost::serialization::singleton<Rpcchannel> SingleRpcchnl;
#define rpcchnl  SingleRpcchnl::get_mutable_instance()

}

#endif

