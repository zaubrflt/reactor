#ifndef REACTOR_BASE_TIMER_H
#define REACTOR_BASE_TIMER_H

#include "base/Timestamp.h"
#include "base/noncopyable.h"
#include "net/Callbacks.h"

#include <atomic>

namespace reactor {

class Timer : public noncopyable {
public:
  // interval --> millisecond
  Timer(const TimerCallback& cb, Timestamp& when, int64_t interval)
    : callback_(std::move(cb)),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0),
      sequence_(++s_numCreated_)
  { }

  void run() const
  {
    callback_();
  }

  Timestamp expiration() const  { return expiration_; }

  bool repeat() const { return repeat_; }

  void restart(Timestamp now);

private:
  const TimerCallback callback_;

  Timestamp expiration_;

  // millisecond
  const int64_t interval_;

  const bool repeat_;

  const int64_t sequence_;

  static std::atomic<int64_t> s_numCreated_;
};

}  // namespace reactor

#endif  // REACTOR_BASE_TIMER_H
