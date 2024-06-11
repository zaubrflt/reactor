// s01/test4.cc用timerfd实现了一个单次触发的定时器, 这个程序利用Channel
// 将timerfd的readable事件转发给timerout()函数

#include "net/EventLoop.h"
#include "net/Channel.h"

#include <sys/timerfd.h>

#include <glog/logging.h>

reactor::net::EventLoop* g_loop;

void timeout()
{
  LOG(INFO) << "Timeout!";
  g_loop->quit();
}

int main(int argc, char const *argv[])
{
  reactor::net::EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0) {
    LOG(ERROR) << "timerfd_create failed, errno: " << errno;
    return -1;
  }
  reactor::net::Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof(howlong));
  howlong.it_value.tv_sec = 5;
  int rc = ::timerfd_settime(timerfd, 0, &howlong, NULL);
  if (rc < 0) {
    LOG(ERROR) << "timerfd_settime failed, errno: " << errno;
    return -1;
  }

  loop.loop();

  ::close(timerfd);

  return 0;
}
