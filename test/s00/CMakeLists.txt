
add_executable(s00_test1 test1.cc)
target_link_libraries(s00_test1
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)

add_executable(s00_test2 test2.cc)
target_link_libraries(s00_test2
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)

add_executable(s00_test3 test3.cc)
target_link_libraries(s00_test3
  ${GLOG_LIBRARY}
  reactor_net
  reactor_base
)