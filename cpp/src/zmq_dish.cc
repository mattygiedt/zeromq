#include <iostream>
#include <string>

#include "common/application_traits.h"
#include "common/signal_handler.h"

auto main(int argc, char** argv) -> int {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " ADDR." << std::endl;
    return 1;
  }

  const auto* addr = argv[1];

  auto message_handler = [](zmq::message_t&& msg) {
    spdlog::info(msg.to_string());
  };

  typename common::DishTraits::DishSocket dish;

  dish.Join(common::RadioTraits::kGroup);
  dish.Bind(addr);
  dish.ProcessMessages(message_handler);
  dish.Close();

  return 0;
}
