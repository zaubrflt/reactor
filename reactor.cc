#include "net/EventLoop.h"

#include <glog/logging.h>
#include <unistd.h>

void threadFunc1()
{
  LOG(INFO) << "threadFunc(): pid = " << getpid()
            << ", tid = " << reactor::tid();
  
  reactor::net::EventLoop loop;
  loop.loop();
}

void test1()
{
  reactor::net::EventLoop loop;

  std::thread t(threadFunc1);
  t.join();
}

reactor::net::EventLoop* g_loop;

void threadFunc2()
{
  g_loop->loop();
}

void test2()
{
  reactor::net::EventLoop loop;
  g_loop = &loop;
  std::thread t(threadFunc2);
  t.join();
}

int main(int argc, char const *argv[])
{
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "main(): pid = " << getpid() << ", tid = " << reactor::tid();

  test1();

  return 0;
}
