#include "net/Channel.h"
#include "net/EventLoop.h"

#include <glog/logging.h>
#include <poll.h>

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

void Channel::handleEvent()
{
  if (revents_ & POLLNVAL) {
    LOG(WARNING) << "Channel::handle_event() POLLNVAL";
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
