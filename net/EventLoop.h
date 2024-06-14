#ifndef REACTOR_NET_EVENTLOOP_H
#define REACTOR_NET_EVENTLOOP_H

#include "base/noncopyable.h"
#include "base/Thread.h"
#include "base/Timestamp.h"
#include "net/TimerId.h"
#include "net/Callbacks.h"

#include <atomic>
#include <memory>
#include <vector>
#include <mutex>

namespace reactor {

namespace net {

class Channel;
class Poller;
class TimerQueue;

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

  // Runs callback at 'time'.
  // Safe to call from other threads.
  TimerId runAt(Timestamp time, TimerCallback cb);

  // Runs callback after @c delay milliseconds.
  // Safe to call from other threads.
  TimerId runAfter(int64_t delay, TimerCallback cb);

  // Runs callback every @c interval milliseconds.
  // Safe to call from other threads.
  TimerId runEvery(int64_t interval, TimerCallback cb);

private:

  void abortNotInLoopThread();

  typedef std::vector<Channel*> ChannelList;

  std::atomic<bool> looping_{false};

  std::atomic<bool> quit_{false};

  std::thread::id threadId_;

  std::unique_ptr<Poller> poller_;

  std::unique_ptr<TimerQueue> timerQueue_;

  ChannelList activeChannels_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_EVENTLOOP_H

