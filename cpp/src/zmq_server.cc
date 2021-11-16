#include <iostream>
#include <string>
#include <thread>

#include "common/application_traits.h"

std::atomic<bool> running_;
std::mutex running_mutex_;
std::condition_variable running_cv_;

auto SignalHandler(int /*unused*/) -> void {
  running_ = false;
  running_cv_.notify_all();
}

auto SetupSignalHandler() -> void {
  struct sigaction sig_int_handler;
  sig_int_handler.sa_handler = SignalHandler;
  sigemptyset(&sig_int_handler.sa_mask);
  sig_int_handler.sa_flags = 0;
  sigaction(SIGINT, &sig_int_handler, nullptr);
}

auto WaitForSignal() -> void {
  running_ = true;
  while (running_) {
    std::unique_lock<decltype(running_mutex_)> lock(running_mutex_);
    running_cv_.wait(lock);
  }
}

auto main(int argc, char** argv) -> int {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " ADDR." << std::endl;
    return 0;
  }

  std::string addr = argv[1];
  spdlog::info("server listen: {}", addr);

  typename common::ServerTraits::ServerSocket server;
  server.Bind(addr);

  server.ProcessMessages([&](const zmq::message_t& msg) {
    spdlog::info(msg.to_string());
    auto str = msg.to_string();
    server.SendMessage(str, msg.routing_id());
  });

  WaitForSignal();

  return 1;
}
