#pragma once

#define ZMQ_BUILD_DRAFT_API 1

#include <thread>

#include "eventpp/eventdispatcher.h"
#include "spdlog/spdlog.h"
#include "zmq.hpp"

namespace common {

class SocketMonitor : public zmq::monitor_t {
 private:
  using Lock = std::unique_lock<std::mutex>;
  using Millis = std::chrono::milliseconds;

 public:
  constexpr static Millis kDefaultTimeout{10000};

  auto on_event_connected(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_connect_delayed(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_connect_retried(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_listening(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_bind_failed(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_accepted(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_accept_failed(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_closed(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_close_failed(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_disconnected(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_handshake_failed_no_detail(const zmq_event_t& event,
                                           const char* addr) -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_handshake_failed_protocol(const zmq_event_t& event,
                                          const char* addr) -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_handshake_failed_auth(const zmq_event_t& event,
                                      const char* addr) -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }
  auto on_event_handshake_succeeded(const zmq_event_t& event, const char* addr)
      -> void override {
    dispatcher_.dispatch(event.event, event, addr);
    dispatcher_.dispatch(ZMQ_EVENT_ALL, event, addr);
  }

  template <typename Callback>
  auto AddListener(const std::uint16_t event_type, Callback&& callback)
      -> void {
    dispatcher_.appendListener(event_type, callback);
  }

 private:
  eventpp::EventDispatcher<std::uint16_t, void(const zmq_event_t&, const char*)>
      dispatcher_;
};

struct BaseProvider {
 private:
  using EventVector = std::vector<zmq::poller_event<>>;

 public:
  constexpr static std::chrono::milliseconds kPollTimeout{100};

  auto SendMessage(const std::string& str, const std::uint32_t routing_id = 0)
      -> zmq::send_result_t {
    if (routing_id == 0) {
      return socket_.send(zmq::buffer(str), zmq::send_flags::dontwait);
    } else {
      zmq::message_t msg{str};
      msg.set_routing_id(routing_id);
      return socket_.send(msg, zmq::send_flags::dontwait);
    }
  }

  template <typename MessageCallback>
  auto ProcessMessages(MessageCallback&& callback) -> void {
    listener_ = std::thread([&]() {
      zmq::poller_t poller;
      poller.add(socket_, zmq::event_flags::pollin);
      EventVector events{poller.size()};

      while (running_) {
        const int num_events = poller.wait_all(events, kPollTimeout);
        for (int i = 0; i < num_events; ++i) {
          zmq::message_t msg;
          auto res = events[i].socket.recv(msg, zmq::recv_flags::dontwait);

          if (res) {
            callback(std::move(msg));
          }
        }
      }
    });
  }

  template <typename MonitorCallback>
  auto Monitor(MonitorCallback&& callback,
               const std::uint16_t event_type = ZMQ_EVENT_ALL) -> void {
    socket_monitor_.AddListener(event_type,
                                std::forward<MonitorCallback>(callback));
  }

  auto Close() -> void {
    running_ = false;
    listener_.join();
  }

 protected:
  BaseProvider(zmq::socket_type type)
      : running_{false}, context_{1}, socket_{context_, type} {}

  std::atomic<bool> running_;
  SocketMonitor socket_monitor_;
  zmq::context_t context_;
  zmq::socket_t socket_;
  std::thread listener_;
  std::thread monitor_;
};

class ClientSocketProvider : public BaseProvider {
 public:
  ClientSocketProvider(const bool monitor_flag = true)
      : BaseProvider(zmq::socket_type::client) {
    if (monitor_flag) {
      monitor_ = std::thread(
          [&]() { socket_monitor_.monitor(socket_, "inproc://monitor"); });
    }
  }

  auto Connect(const std::string& addr) -> void {
    spdlog::info("socket_.connect({})", addr);
    socket_.connect(addr);
    running_ = true;
  }
};

class ServerSocketProvider : public BaseProvider {
 public:
  ServerSocketProvider(const bool monitor_flag = true)
      : BaseProvider(zmq::socket_type::server) {
    if (monitor_flag) {
      monitor_ = std::thread(
          [&]() { socket_monitor_.monitor(socket_, "inproc://monitor"); });
    }
  }

  auto Bind(const std::string& addr) -> void {
    spdlog::info("socket_.bind({})", addr);
    socket_.bind(addr);
    running_ = true;
  }
};

}  // namespace common
