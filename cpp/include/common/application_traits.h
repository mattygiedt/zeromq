#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "common/socket_providers.h"
#include "common/time_util.h"
#include "spdlog/spdlog.h"

namespace common {

struct CommonTraits {
  static constexpr auto kLoop = 1000000;
  static constexpr auto kLoopWait = std::chrono::milliseconds(1);
};

struct ClientTraits : public CommonTraits {
  using ClientSocket = ClientSocketProvider;
};

struct ServerTraits : public CommonTraits {
  using ServerSocket = ServerSocketProvider;
};

struct RadioTraits : public CommonTraits {
  inline static const std::string kGroup{"//eqty/aapl"};
  inline static const std::string kAddr{"udp://239.192.0.1:7500"};
  using RadioSocket = RadioSocketProvider;
};

struct DishTraits : public CommonTraits {
  using DishSocket = DishSocketProvider;
};

struct SubTraits : public CommonTraits {
  using SubSocket = SubSocketProvider;
};

struct PubTraits : public CommonTraits {
  using PubSocket = PubSocketProvider;
};

}  // namespace common
