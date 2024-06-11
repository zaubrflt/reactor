// test3是一个负面测试，在主线程创建两个EventLoop对象，程序会异常终止。

#include "net/EventLoop.h"

#include <glog/logging.h>
#include <unistd.h>

// void threadFunc()
// {
//   LOG(INFO) << "threadFunc(): pid = " << getpid()
//             << ", tid = " << reactor::tid();
  
//   reactor::net::EventLoop loop;
//   loop.loop();
// }

int main(int argc, char const *argv[])
{
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "main(): pid = " << getpid() << ", tid = " << reactor::tid();

  reactor::net::EventLoop loop1;

  reactor::net::EventLoop loop2;

  loop1.loop();

  loop2.loop();

  return 0;
}