#include "net/EventLoop.h"
#include "net/EventLoopThread.h"

#include <unistd.h>

#include <glog/logging.h>

void runInThread()
{
  LOG(INFO) << "runInThread(): pid = " << getpid()
            << ", tid = " << reactor::tid();
}

int main()
{
  LOG(INFO) << "main(): pid = " << getpid()
            << ", tid = " << reactor::tid();

  reactor::net::EventLoopThread loopThread;
  reactor::net::EventLoop* loop = loopThread.startLoop();
  loop->runInLoop(runInThread);
  sleep(1);
  loop->runAfter(2000, runInThread);
  sleep(3);
  loop->quit();

  LOG(INFO) << "exit main()";
}
