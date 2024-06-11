set(base_SRCS
  Thread.cc
)

add_library(reactor_base ${base_SRCS})

file(GLOB HEADERS "*.h")

