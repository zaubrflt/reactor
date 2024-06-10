#include "EventLoop.h"

#include <glog/logging.h>

#include <chrono>

#include <assert.h>

using namespace reactor;

using namespace reactor::net;

thread_local EventLoop* t_loopInThisThread = nullptr;

EventLoop::EventLoop()
  : threadId_(tid())
{
  LOG(INFO) << "EventLoop created " << this << " in thread " << threadId_;
  if (t_loopInThisThread) {
    LOG(FATAL) << "Another EventLoop " << t_loopInThisThread
               << " exists in this thread " << threadId_;
  } else {
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop()
{
  assert(!looping_);
  t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
  assert(!looping_.load(std::memory_order_relaxed));
  assertInLoopThread();
  looping_.store(true, std::memory_order_relaxed);

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  LOG(INFO) << "EventLoop " << this << " stop looping";
  looping_.store(false, std::memory_order_relaxed);
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
  return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread()
{
  LOG(FATAL) << "EventLoop::abortNotInLoopThread - EventLoop " << this
             << " was created in threadId_ = " << threadId_
             << ", current thread id = " <<  tid();
}

