#include "net/EventLoopThread.h"
#include "net/EventLoop.h"

namespace reactor {

namespace net {

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb,
                                 const std::string& name)
  : loop_(nullptr),
    thread_(std::bind(&EventLoopThread::threadFunc, this), name),
    callback_(cb)
{
}

EventLoopThread::~EventLoopThread()
{
  if (loop_ != nullptr) {
    loop_->quit();
    thread_.join();
  }
}

EventLoop* EventLoopThread::startLoop()
{
  EventLoop* loop = nullptr;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (loop_ == nullptr) {
      cond_.wait(lock);
    }
    loop = loop_;
  }

  return loop;
}

void EventLoopThread::threadFunc()
{
  EventLoop loop;

  if (callback_) {
    callback_(&loop);
  }

  {
    std::lock_guard<std::mutex> lock(mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }

  loop.loop();
  std::lock_guard<std::mutex> lock(mutex_);
  loop_ = nullptr;
}

}  // namespace net

}  // namespace reactor
