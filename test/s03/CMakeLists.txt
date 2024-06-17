add_executable(s03_test5 test5.cc)
target_link_libraries(s03_test5
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)

add_executable(s03_test6 test6.cc)
target_link_libraries(s03_test6
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)
