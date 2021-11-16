#pragma once

#include <condition_variable>
#include <csignal>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "common/socket_providers.h"
#include "common/time_util.h"
#include "spdlog/spdlog.h"

namespace common {

struct CommonTraits {};

struct ClientTraits : public CommonTraits {
  static constexpr auto kQueueWait = std::chrono::milliseconds(100);
  using ClientSocket = ClientSocketProvider;
};

struct ServerTraits : public CommonTraits {
  static constexpr auto kQueueWait = std::chrono::milliseconds(100);
  using ServerSocket = ServerSocketProvider;
};

}  // namespace common
