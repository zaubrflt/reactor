#include "net/TimerQueue.h"
#include "net/EventLoop.h"
#include "base/Timer.h"

#include <glog/logging.h>

#include <assert.h>
#include <sys/timerfd.h>
#include <iostream>

namespace reactor {

namespace net {

int createTimerfd()
{
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0) {
    LOG(FATAL) << "Failed in timerfd_create";
  }
  return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
  int64_t microseconds = durationMicros(when - Clock::now());
  if (microseconds < 100) {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(microseconds / kMicroToSec);
  ts.tv_nsec = static_cast<long>((microseconds % kMicroToSec) * 1000);
  return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof(newValue));
  bzero(&oldValue, sizeof(oldValue));
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) {
    LOG(ERROR) << "timerfd_settime()";
  }
}

void readTimerfd(int timerfd, Timestamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
  LOG(INFO) << "TimerQueue::handleRead() " << howmany
            << " at " << timeFormat(now);
  if (n != sizeof(howmany)) {
    LOG(ERROR) << "TimerQueue::handleRead() reads " << n
               << " bytes instead of 8";
  }
}

TimerQueue::TimerQueue(EventLoop *loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_)
{
  timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
  for (auto it = timers_.begin(); it != timers_.end(); it++) {
    delete it->second;
  }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb,
                             Timestamp& when,
                             int64_t interval)
{
  // auto timer = std::make_shared<Timer>(cb, when, interval);
  Timer* timer = new Timer(cb, when, interval);
  loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  loop_->assertInLoopThread();
  bool earliestChanged = insert(timer);
  if (earliestChanged) {
    resetTimerfd(timerfd_, timer->expiration());
  }
}

void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  Timestamp now = Clock::now();
  readTimerfd(timerfd_, now);

  std::vector<Entry> expired = getExpired(now);

  // safe to callback outside critical section
  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); it++)
  {
    it->second->run();
  }

  reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(const Timestamp& now)
{
  std::vector<Entry> expired;
  Entry sentry = std::make_pair(now, reinterpret_cast<Timer *>(UINTPTR_MAX));
  // sentry的选取让set::lower_bound()返回的是第一个未到期的Timer的迭代器.
  // 因此下面的assert断言中是<而非<=
  auto it = timers_.lower_bound(sentry);
  assert(it == timers_.end() || now < it->first);
  std::copy(timers_.begin(), it, back_inserter(expired));
  timers_.erase(timers_.begin(), it);
  // RVO
  return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, const Timestamp& now)
{
  Timestamp nextExpire;
  for (auto it = expired.begin(); it != expired.end(); it++) {
    if (it->second->repeat()) {
      it->second->restart(now);
      insert(it->second);
    }
  }
  if (!timers_.empty()) {
    nextExpire = timers_.begin()->second->expiration();
  }
  if (nextExpire.time_since_epoch().count() > 0) {
    resetTimerfd(timerfd_, nextExpire);
  }
}

bool TimerQueue::insert(Timer* timer)
{
  bool earliestChanged = false;
  Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first) {
    earliestChanged = true;
  }
  auto result = timers_.insert(std::make_pair(when, timer));
  assert(result.second);
  return earliestChanged;
}

}  // namespace net

}  // namespace reactor
