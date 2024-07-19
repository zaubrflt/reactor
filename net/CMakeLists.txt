set(net_SRCS
  EventLoop.cc
  Channel.cc
  Poller.cc
  TimerQueue.cc
  EventLoopThread.cc
  SocketOps.cc
  Socket.cc
  InetAddress.cc
  Acceptor.cc
  TcpServer.cc
  TcpConnection.cc
)

set(HEADERS
  EventLoop.h
  Channel.h
  Poller.h
  TimerId.h
  TimerQueue.h
  Callbacks.h
  EventLoopThread.h
  SocketOps.h
  Socket.h
  InetAddress.h
  Acceptor.h
  TcpServer.h
  TcpConnection.h
)

add_library(reactor_net ${net_SRCS})
target_link_libraries(reactor_net reactor_base)
