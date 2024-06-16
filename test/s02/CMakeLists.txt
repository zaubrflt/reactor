add_executable(s02_test4 test4.cc)
target_link_libraries(s02_test4
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)
