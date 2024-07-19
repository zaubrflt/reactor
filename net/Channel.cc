#include "net/Channel.h"
#include "net/EventLoop.h"

#include <glog/logging.h>

#include <poll.h>
#include <assert.h>

namespace reactor {

namespace net {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
  : loop_(loop),
    fd_(fd)
{
}

Channel::~Channel()
{
  assert(!eventHandling_);
}

void Channel::handleEvent()
{
  eventHandling_ = true;
  if (revents_ & POLLNVAL) {
    LOG(WARNING) << "Channel::handle_event() POLLNVAL";
  }

  if ((revents_ && POLLHUP) && !(revents_ & POLLIN)) {
    LOG(WARNING) << "Channel::handle_event() POLLHUP";
    if (closeCallback_) closeCallback_();
  }
  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_) readCallback_();
  }
  if (revents_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }
}

void Channel::update()
{
  loop_->updateChannel(this);
}

}  // namespace net

}  // namespace reactor
