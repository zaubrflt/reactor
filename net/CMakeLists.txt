set(net_SRCS
  EventLoop.cc
  Channel.cc
  Poller.cc
  TimerQueue.cc
)

set(HEADERS
  EventLoop.h
  Channel.h
  Poller.h
  TimerId.h
  TimerQueue.h
  Callbacks.h
)

add_library(reactor_net ${net_SRCS})
target_link_libraries(reactor_net reactor_base)
