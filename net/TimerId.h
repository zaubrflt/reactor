#ifndef REACTOR_NET_TIMERID_H
#define REACTOR_NET_TIMERID_H

#include <stdint.h>

namespace reactor {

class Timer;

class TimerId {
public:
  TimerId() { }

  TimerId(Timer* timer, int64_t seq = 0)
    : timer_(timer),
      sequence_(seq)
  {
  }

  friend class TimerQueue;

private:
  Timer* timer_{nullptr};

  int64_t sequence_{0};
};

}  // namespace reactor

#endif  // REACTOR_NET_TIMERID_H
