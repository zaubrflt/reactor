#ifndef REACTOR_NET_EVENTLOOP_H
#define REACTOR_NET_EVENTLOOP_H

#include "base/noncopyable.h"
#include "base/Thread.h"

#include <atomic>
#include <memory>
#include <vector>

namespace reactor {

namespace net {

class Channel;
class Poller;

class EventLoop : public noncopyable {
public:
  EventLoop();

  ~EventLoop();

  void loop();

  void updateChannel(Channel* channel);

  void assertInLoopThread()
  {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() const { return tid() == threadId_; }

  static EventLoop* getEventLoopOfCurrentThread();

  void quit();

private:

  void abortNotInLoopThread();

private:
  typedef std::vector<Channel*> ChannelList;

  std::atomic<bool> looping_{false};

  std::atomic<bool> quit_{false};

  std::thread::id threadId_;

  std::unique_ptr<Poller> poller_{nullptr};

  ChannelList activeChannels_;
};

}

}

#endif

