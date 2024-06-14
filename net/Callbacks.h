#ifndef REACTOR_NET_CALLBACKS_H
#define REACTOR_NET_CALLBACKS_H

#include <functional>

namespace reactor {

typedef std::function<void()> TimerCallback;

}  // namespace reactor

#endif  // REACTOR_NET_CALLBACKS_H