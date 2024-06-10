#ifndef REACTOR_NET_EVENTLOOP_H
#define REACTOR_NET_EVENTLOOP_H

#include "base/noncopyable.h"
#include "base/Thread.h"

#include <atomic>

namespace reactor {

namespace net {

class EventLoop : public noncopyable {
public:
  EventLoop();

  ~EventLoop();

  void loop();

  void assertInLoopThread()
  {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() const { return tid() == threadId_; }

  static EventLoop* getEventLoopOfCurrentThread();

private:

  void abortNotInLoopThread();

private:
  std::atomic<bool> looping_{false};

  std::thread::id threadId_;
};

}

}

#endif

