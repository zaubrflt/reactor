#ifndef REACTOR_NET_TIMERQUEUE_H
#define REACTOR_NET_TIMERQUEUE_H

#include "base/noncopyable.h"
#include "base/Timestamp.h"
#include "net/TimerId.h"
#include "net/Callbacks.h"
#include "net/Channel.h"

#include <memory>
#include <set>
#include <vector>

namespace reactor {

namespace net {

class EventLoop;

class TimerQueue : public noncopyable {
public:
  explicit TimerQueue(EventLoop* loop);

  ~TimerQueue();

  // Schedules the callback to be run at given time, repeats
  // if @c interval (millisecond) > 0.
  // Must be tread safe. Usually be called from other threads.
  TimerId addTimer(const TimerCallback& cb, Timestamp& when, int64_t interval);

private:
  // TODO: 这里考虑把裸指针替换为智能指针
  typedef std::pair<Timestamp, Timer*> Entry;

  typedef std::set<Entry> TimerList;

  // called when timerfd alarms
  void handleRead();

  // 从timers_中移除已到期的Timer, 并通过vector返回它们
  std::vector<Entry> getExpired(const Timestamp& now);

  void reset(const std::vector<Entry>& expired, const Timestamp& now);

  bool insert(Timer* timer);

  EventLoop* loop_{nullptr};

  const int timerfd_;

  Channel timerfdChannel_;

  TimerList timers_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_TIMERQUEUE_H