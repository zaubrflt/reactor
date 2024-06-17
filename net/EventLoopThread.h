#ifndef REACTOR_NET_EVENTLOOPTHREAD_H
#define REACTOR_NET_EVENTLOOPTHREAD_H

#include "base/noncopyable.h"

#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <string>

namespace reactor {

namespace net {

class EventLoop;

class EventLoopThread : public noncopyable {
 public:
  typedef std::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                  const std::string& name = "");

  ~EventLoopThread();

  EventLoop* startLoop();

 private:
  void threadFunc();

  EventLoop* loop_;

  std::thread thread_;

  std::mutex mutex_;

  std::condition_variable cond_ ;

  ThreadInitCallback callback_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_EVENTLOOPTHREAD_H

