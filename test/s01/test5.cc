#include "net/EventLoop.h"

#include <glog/logging.h>

#include <string>

int cnt = 0;
reactor::net::EventLoop* g_loop;

void printTid()
{
  LOG(INFO) << "pid = " << getpid() << ", tid = " << reactor::tid();
  LOG(INFO) << "now " << reactor::timeFormat(reactor::Clock::now());
}

void print(const std::string& msg)
{
  LOG(INFO) << "msg " << reactor::timeFormat(reactor::Clock::now())
            << " " << msg;
  if (++cnt == 20) {
    g_loop->quit();
  }
}

int main(int argc, char const *argv[])
{
  google::InitGoogleLogging(argv[0]);

  printTid();
  reactor::net::EventLoop loop;
  g_loop = &loop;

  print("main");
  loop.runAfter(1000, std::bind(print, "once1"));
  loop.runAfter(1500, std::bind(print, "once1.5"));
  loop.runAfter(2500, std::bind(print, "once2.5"));
  loop.runAfter(3500, std::bind(print, "once3.5"));
  loop.runEvery(2000, std::bind(print, "every2"));
  loop.runEvery(3000, std::bind(print, "every3"));

  loop.loop();
  print("main loop exits");
  sleep(1);
}