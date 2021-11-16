#pragma once

#include <chrono>

namespace common {

struct TimeUtil {
  using ClockType = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<ClockType>;
  using Timestamp = std::uint64_t;

  static auto EpochNanos() -> Timestamp {
    return ClockType::now().time_since_epoch().count();
  }
};
}  // namespace common
