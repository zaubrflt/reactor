#ifndef REACTOR_BASE_TIMESTAMP_H
#define REACTOR_BASE_TIMESTAMP_H

#include <chrono>
#include <string>

namespace reactor {

const int64_t kMicroToMilli = 1000;
const int64_t kMicroToSec = 1000 * 1000;
const int64_t kMilliToSec = 1000;

typedef std::chrono::high_resolution_clock Clock;

typedef std::chrono::time_point<Clock> Timestamp;

inline Timestamp addTime(Timestamp timestamp, int64_t millisecond)
{
  std::chrono::milliseconds delta(millisecond);
  return Timestamp(timestamp.time_since_epoch() + delta);
}

inline std::string timeFormat(Timestamp timepoint)
{
  std::time_t t = Clock::to_time_t(timepoint);
  char buf[20];
  strftime(buf, 20, "%d.%m.%Y %H:%M:%S", localtime(&t));
  return buf;
}

template <typename T>
int64_t durationMicros(T d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

template <typename T>
int durationMillis(T d)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

}  // namespace reactor

#endif  // REACTOR_BASE_TIMESTAMP_H
