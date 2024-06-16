#include "net/EventLoop.h"
#include "net/Channel.h"
#include "net/Poller.h"
#include "net/TimerQueue.h"

#include <glog/logging.h>

#include <assert.h>
#include <sys/eventfd.h>

namespace reactor {

namespace net {

thread_local EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG(ERROR) << "Failed in eventfd";
    abort();
  }
  return evtfd;
}

EventLoop::EventLoop()
  : threadId_(tid()),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_))
{
  LOG(INFO) << "EventLoop created " << this << " in thread " << threadId_;
  if (t_loopInThisThread) {
    LOG(FATAL) << "Another EventLoop " << t_loopInThisThread
               << " exists in this thread " << threadId_;
  } else {
    t_loopInThisThread = this;
  }
  wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
  wakeupChannel_->enableReading();
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
    doPendingFunctors();
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
  if (!isInLoopThread()) {
    wakeup();
  }
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

void EventLoop::runInLoop(const Functor& cb)
{
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(cb);
  }
}

void EventLoop::queueInLoop(const Functor& cb)
{
  {
    std::lock_guard<std::mutex> lock(mutex_);
    pendingFunctors_.push_back(cb);
  }
  if (!isInLoopThread() || callingPendingFunctors_) {
    // 由于doPendingFunctors()调用的Functor可能再调用queueInLoop(cb),
    // 这时queueInLoop()就必须wakeup()
    wakeup();
  }
}

void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof one);
  if (n != sizeof(one)) {
    LOG(ERROR) << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof one);
  if (n != sizeof(one)) {
    LOG(ERROR) << "EventLoop::handleRead() reads " << n
               << " bytes instead of 8";
  }
}

void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  callingPendingFunctors_.store(true, std::memory_order_relaxed);
  {
    std::lock_guard<std::mutex> lock(mutex_);
    functors.swap(pendingFunctors_);
  }
  for (std::size_t i = 0; i < functors.size(); i++) {
    functors[i]();
  }
  callingPendingFunctors_.store(false, std::memory_order_relaxed);
}

}  // namespace net

}  // namespace reactor
