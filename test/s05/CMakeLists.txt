add_executable(s05_test8 test8.cc)
target_link_libraries(s05_test8
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)
