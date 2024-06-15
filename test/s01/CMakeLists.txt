add_executable(s01_test3 test3.cc)
target_link_libraries(s01_test3
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)

add_executable(s01_test4 test4.cc)
target_link_libraries(s01_test4
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)
