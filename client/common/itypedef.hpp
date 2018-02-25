#ifndef RPC_ITYPEDEF_H__
#define RPC_ITYPEDEF_H__

#include <functional>
#include <boost/serialization/singleton.hpp>
#include "singleton.hpp"

namespace sindia 
{
class Connection;
class Dispatcher;

using Connection_weak = std::weak_ptr<Connection>;
using Connection_ptr = std::shared_ptr<Connection>;
using CloseCallback = std::function<void (const Connection_ptr&)>;

using Handler_t = std::function<std::string (std::string)>;
using Callmap_t = std::unordered_map<size_t, Handler_t>;

typedef Singleton<Dispatcher> SingleDispatch;
}

#endif

