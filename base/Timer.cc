#include "base/Timer.h"

#include <glog/logging.h>

namespace reactor {

std::atomic<int64_t> Timer::s_numCreated_;

void Timer::restart(Timestamp now)
{
  if (repeat_) {
    expiration_ = addTime(now, interval_);
  }
}

}  // namespace reactor
