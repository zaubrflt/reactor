set(net_SRCS
  EventLoop.cc
)

add_library(reactor_net ${net_SRCS})
target_link_libraries(reactor_net reactor_base)

set(HEADERS
  EventLoop.h
)


