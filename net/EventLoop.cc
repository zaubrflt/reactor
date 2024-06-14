#include "net/EventLoop.h"
#include "net/Channel.h"
#include "net/Poller.h"
#include "net/TimerQueue.h"

#include <glog/logging.h>

#include <chrono>
#include <assert.h>

namespace reactor {

namespace net {

thread_local EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

EventLoop::EventLoop()
  : threadId_(tid()),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this))
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
  quit_.store(false, std::memory_order_relaxed);

  while (!quit_.load(std::memory_order_relaxed)) {
    activeChannels_.clear();
    poller_->poll(kPollTimeMs, &activeChannels_);
    for (auto it = activeChannels_.begin(); it != activeChannels_.end(); it++) {
      (*it)->handleEvent();
    }
  }

  LOG(INFO) << "EventLoop " << this << " stop looping";
  looping_.store(false, std::memory_order_relaxed);
}

void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller_->updateChannel(channel);
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

void EventLoop::quit()
{
  quit_.store(true, std::memory_order_relaxed);
}

TimerId EventLoop::runAt(Timestamp time, TimerCallback cb)
{
  return timerQueue_->addTimer(std::move(cb), time, 0);
}

TimerId EventLoop::runAfter(int64_t delay, TimerCallback cb)
{
  Timestamp time(addTime(Clock::now(), delay));
  return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(int64_t interval, TimerCallback cb)
{
  Timestamp time(addTime(Clock::now(), interval));
  return timerQueue_->addTimer(std::move(cb), time, interval);
}

}  // namespace net

}  // namespace reactor
