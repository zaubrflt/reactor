#ifndef REACTOR_NET_CALLBACKS_H
#define REACTOR_NET_CALLBACKS_H

#include <functional>
#include <memory>
#include <sys/types.h>

namespace reactor {

namespace net {

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;

typedef std::function<void (const TcpConnectionPtr&,
                            const char* data,
                            ssize_t len)> MessageCallback;

typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;

}  // namespace net

typedef std::function<void()> TimerCallback;

}  // namespace reactor

#endif  // REACTOR_NET_CALLBACKS_H
