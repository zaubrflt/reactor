set(base_SRCS
  Thread.cc
  Timer.cc
)

set(HEADERS
  Thread.h
  Timer.h
  Timestamp.h
)

add_library(reactor_base ${base_SRCS})
