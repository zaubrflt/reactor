#pragma once

#include <chrono>

namespace reactor {

typedef std::chrono::high_resolution_clock Clock;

typedef std::chrono::time_point<Clock> Timestamp;

}