#ifndef REACTOR_BASE_TIME_H
#define REACTOR_BASE_TIME_H

#include <chrono>

namespace reactor {

typedef std::chrono::high_resolution_clock Clock;

typedef std::chrono::time_point<Clock> Timestamp;

}  // namespace reactor

#endif  // REACTOR_BASE_TIME_H
