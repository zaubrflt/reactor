add_executable(s04_test7 test7.cc)
target_link_libraries(s04_test7
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)
