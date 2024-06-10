# include(CheckFunctionExists)

# check_function_exists(accept4 HAVE_ACCEPT4)
# if(NOT HAVE_ACCEPT4)
#   set_source_files_properties(SocketsOps.cc PROPERTIES COMPILE_FLAGS "-DNO_ACCEPT4")
# endif()

set(net_SRCS
  EventLoop.cc
)

add_library(reactor_net ${net_SRCS})
target_link_libraries(reactor_net reactor_base)

set(HEADERS
  EventLoop.h
)

# add_subdirectory(http)
# add_subdirectory(inspect)

