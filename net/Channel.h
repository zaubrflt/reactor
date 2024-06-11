#pragma once

#include "base/noncopyable.h"

#include <functional>

namespace reactor {

namespace net {

class EventLoop;

// 每个Channel对象自始至终只属于一个EventLoop, 因此每个Channel对象都只
// 属于某一个IO线程. 每个Channel对象自始至终只负责一个文件描述符(fd)的IO
// 事件分发, 但它并不拥有这个fd, 也不会在析构的时候关闭这个fd. Channel会
// 不同的IO时间分发为不同的回调, 例如ReadCallback, WriteCallback等.
// 用户一般不直接使用Channel, 而会使用更上层的封装, 如TcpConnection.
// Channel的生命期由其owner class负责管理.
class Channel : public noncopyable {
public:
  typedef std::function<void()> EventCallback;

  Channel(EventLoop* loop, int fd);

  // Channel的核心, 由EventLoop::loop()调用, 功能是
  // 根据revents_的值分别调用不同的用户回调.
  void handleEvent();

  void setReadCallback(const EventCallback& cb)
  { readCallback_ = std::move(cb); }

  void setWriteCallback(const EventCallback& cb)
  { writeCallback_ = std::move(cb); }

  void setErrorCallback(const EventCallback& cb)
  { errorCallback_ = std::move(cb); }

  int fd() const { return fd_; }

  int events() const { return events_; }

  void setRevents(int revt) { revents_ = revt; }

  bool isNoneEvent() const { return events_ == kNoneEvent; }

  void enableReading()
  {
    events_ |= kReadEvent;
    update();
  }

  int index() { return index_; }

  void setIndex(int idx) { index_ = idx; }

  EventLoop* ownerLoop() { return loop_; }

private:
  void update();

  static const int kNoneEvent;

  static const int kReadEvent;
  
  static const int kWriteEvent;

  EventLoop* loop_{nullptr};

  const int fd_;

  // Channel关心的IO事件, 由用户设置
  int events_{0};

  // 目前活动的事件, 由EventLoop/Poller设置
  int revents_{0};

  // used by Poller
  int index_{-1};

  EventCallback readCallback_;

  EventCallback writeCallback_;

  EventCallback errorCallback_;
};

}

}