#pragma once

#define ZMQ_BUILD_DRAFT_API 1

#include <thread>

#include "spdlog/spdlog.h"
#include "zmq.hpp"

namespace common {

class SocketMonitor : public zmq::monitor_t {
 private:
  using Lock = std::unique_lock<std::mutex>;
  using Millis = std::chrono::milliseconds;

 public:
  constexpr static Millis kDefaultTimeout{10000};

  void on_event_connected(const zmq_event_t& /*unused*/,
                          const char* addr) override {
    spdlog::info("connection established: {}", addr);
    connected_ = true;
    cv_.notify_all();
  }

  void on_event_disconnected(const zmq_event_t& /*unused*/,
                             const char* addr) override {
    spdlog::info("connection closed: {}", addr);
    connected_ = false;
  }

  auto WaitForConnect(const Millis& timeout = kDefaultTimeout) -> bool {
    if (connected_) {
      return true;
    }

    Lock lk(mtx_);
    cv_.wait_for(lk, timeout);
    return connected_;
  }

  auto IsConnected() -> bool { return connected_; }

 private:
  std::mutex mtx_;
  std::condition_variable cv_;
  std::atomic<bool> connected_{false};
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
            callback(msg);
          }
        }
      }
    });
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
 private:
  using Millis = std::chrono::milliseconds;
  using EventVector = std::vector<zmq::poller_event<>>;

 public:
  constexpr static Millis kConnectTimeout{1000};

  ClientSocketProvider() : BaseProvider(zmq::socket_type::client) {
    monitor_ = std::thread(
        [&]() { socket_monitor_.monitor(socket_, "inproc://monitor"); });
  }

  auto Connect(const std::string& addr, const Millis& timeout = kConnectTimeout)
      -> bool {
    spdlog::info("socket_.connect({})", addr);
    socket_.connect(addr);
    running_ = socket_monitor_.WaitForConnect(timeout);
    return running_;
  }
};

class ServerSocketProvider : public BaseProvider {
 public:
  ServerSocketProvider() : BaseProvider(zmq::socket_type::server) {
    monitor_ = std::thread(
        [&]() { socket_monitor_.monitor(socket_, "inproc://monitor"); });
  }

  auto Bind(const std::string& addr) -> void {
    spdlog::info("socket_.bind({})", addr);
    socket_.bind(addr);
    running_ = true;
  }
};

}  // namespace common
