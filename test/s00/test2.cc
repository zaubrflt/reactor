// test2是个负面测试，它在主线程创建了EventLoop对象，却试图在另一个
// 线程调用其EventLoop::loop()，程序会因断言失效而异常终止。

#include "net/EventLoop.h"

#include <glog/logging.h>
#include <unistd.h>

reactor::net::EventLoop* g_loop;

void threadFunc()
{
  g_loop->loop();
}

int main(int argc, char const *argv[])
{
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "main(): pid = " << getpid() << ", tid = " << reactor::tid();

  reactor::net::EventLoop loop;
  g_loop = &loop;
  std::thread t(threadFunc);
  t.join();

  return 0;
}
