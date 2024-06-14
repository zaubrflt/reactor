#ifndef REACTOR_NET_POLLER_H
#define REACTOR_NET_POLLER_H

#include "base/noncopyable.h"
#include "base/Timestamp.h"

#include <vector>
#include <chrono>
#include <map>

struct pollfd;

namespace reactor {

namespace net {

class Channel;
class EventLoop;

// Poller是IO multiplexing的封装. Poller是EventLoop的间接成员,
// 只供其owner EventLoop在IO线程调用, 因此无需枷锁. 其生命期与
// EventLoop相等. Poller并不拥有Channel, Channel在析构之前必须
// 自己unregister(EventLoop::removeChannel()), 避免悬空指针.
class Poller : public noncopyable {
public:
  typedef std::vector<Channel*> ChannelList;

  Poller(EventLoop* loop);

  ~Poller();

  // Polls the I/O events.
  // Must be called in the loop thread.
  Timestamp poll(int timeoutMs, ChannelList* activeChannels);

  // 更新和维护pollfds_数组
  void updateChannel(Channel* channel);

  void assertInLoopThread();

private:
  // 找出有活动事件的fd, 把它对应的Channel填入activeChannels
  void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

  typedef std::vector<struct pollfd> PollFdList;

  typedef std::map<int, Channel*> ChannelMap;

private:
  EventLoop* ownerLoop_{nullptr};

  PollFdList pollfds_;

  ChannelMap channels_;
};

}  // namespace net

}  // namespace reactor

#endif  // REACTOR_NET_POLLER_H
