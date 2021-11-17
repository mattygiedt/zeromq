#pragma once

#include <atomic>
#include <condition_variable>
#include <csignal>
#include <mutex>

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
