#include "net/Poller.h"
#include "net/EventLoop.h"
#include "net/Channel.h"

#include <glog/logging.h>
#include <poll.h>
#include <assert.h>

namespace reactor {

namespace net {

Poller::Poller(EventLoop *loop)
  : ownerLoop_(loop)
{
}

Poller::~Poller()
{
}

Timestamp Poller::poll(int timeoutMs, ChannelList *activeChannels)
{
  // Poller的核心功能, 调用poll获得当前活动的IO事件, 然后填充调用方传入的
  // activeChannels, 并返回poll return的时刻.
  int numEvents = ::poll(pollfds_.data(), pollfds_.size(), timeoutMs);
  Timestamp now = Clock::now();
  if (numEvents > 0) {
    LOG(INFO) << numEvents << " events happended";
    fillActiveChannels(numEvents, activeChannels);
  } else if (numEvents == 0) {
    LOG(INFO) << "nothing happended";
  } else {
    LOG(ERROR) << "Poller::poll()";
  }
  return now;
}

void Poller::updateChannel(Channel *channel)
{
  assertInLoopThread();
  LOG(INFO) << "fd = " << channel->fd() << " events = " << channel->events();
  if (channel->index() < 0) {
    // 添加新的Channel
    assert(channels_.find(channel->fd()) == channels_.end());
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size()) - 1;
    channel->setIndex(idx);
    channels_[pfd.fd] = channel;
  } else {
    // 更新已有的Channel
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    struct pollfd& pfd = pollfds_[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -1);
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if (channel->isNoneEvent()) {
      // 如果某个Channel暂时不关心任何事件, 就把pollfd.fd设为-1, 让
      // poll忽略这个pollfd. 这里不能改为把pollfd.events设为0, 这样
      // 无法屏蔽POLLERR事件.
      pfd.fd = -1;
    }
  }
}

void Poller::assertInLoopThread()
{
  assert(ownerLoop_);
  ownerLoop_->assertInLoopThread();
}

void Poller::fillActiveChannels(int numEvents,
                                ChannelList *activeChannels) const
{
  for (auto pfd = pollfds_.begin();
       pfd != pollfds_.end() && numEvents > 0; pfd++) {
    // 注意这里不能一边遍历pollfds_, 一边调用Channel::handleEvent(), 因为后者
    // 会添加或删除Channel, 从而造成pollfds_在遍历期间改变大小, 这是非常危险的.
    if (pfd->revents > 0) {
      --numEvents;
      auto ch = channels_.find(pfd->fd);
      assert(ch != channels_.end());
      Channel* channel = ch->second;
      assert(channel->fd() == pfd->fd);
      // 当前活动事件revents会保存在Channel中, 供Channel::handleEvent()使用.
      channel->setRevents(pfd->revents);
      activeChannels->push_back(channel);
    }
  }
}

}  // namespace net

}  // namespace reactor
