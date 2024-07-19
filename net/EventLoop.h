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
  typedef std::function<void()> Functor;

  EventLoop();

  ~EventLoop();

  void loop();

  void updateChannel(Channel* channel);

  void removeChannel(Channel* channel);

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

  // 在其IO线程内执行某个用户任务回调. 如果用户在当前IO线程调用这个函数, 回调会同步进行;
  // 如果用户在其他线程调用runInLoop(), cb会被加入队列, IO线程会被唤醒来调用这个Functor.
  void runInLoop(const Functor& cb);

  void queueInLoop(const Functor& cb);

private:

  void abortNotInLoopThread();

  // waked up
  void handleRead();

  void doPendingFunctors();

  void wakeup();

  typedef std::vector<Channel*> ChannelList;

  std::atomic<bool> looping_{false};

  std::atomic<bool> quit_{false};

  std::atomic<bool> callingPendingFunctors_;

  std::thread::id threadId_;

  std::unique_ptr<Poller> poller_;

  std::unique_ptr<TimerQueue> timerQueue_;

  int wakeupFd_;

  // 处理wakeupFd_上的readable事件, 将事件分发至handleRead()函数
  std::unique_ptr<Channel> wakeupChannel_;

  ChannelList activeChannels_;

  std::mutex mutex_;

  std::vector<Functor> pendingFunctors_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_EVENTLOOP_H

