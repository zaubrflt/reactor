// test1在主线程和子线程分别创建一个EventLoop，程序正常运行退出。

#include "net/EventLoop.h"

#include <glog/logging.h>
#include <unistd.h>

void threadFunc()
{
  LOG(INFO) << "threadFunc(): pid = " << getpid()
            << ", tid = " << reactor::tid();

  reactor::net::EventLoop loop;
  loop.loop();
}

int main(int argc, char const *argv[])
{
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "main(): pid = " << getpid() << ", tid = " << reactor::tid();

  reactor::net::EventLoop loop;

  std::thread t(threadFunc);
  t.join();

  return 0;
}