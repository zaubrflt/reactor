set(base_SRCS
  Thread.cc
)

add_library(reactor_base ${base_SRCS})
# target_link_libraries(base pthread rt)

file(GLOB HEADERS "*.h")

