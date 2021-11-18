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

  auto connected_event = [](const zmq_event_t& event, const char* addr) {
    spdlog::info("sub connected: addr {}, fd {}", addr, event.value);
  };

  auto message_handler = [](zmq::message_t&& msg) {
    spdlog::info(msg.to_string());
  };

  typename common::SubTraits::SubSocket sub;

  sub.Monitor(connected_event, ZMQ_EVENT_CONNECTED);
  sub.Subscribe("");
  sub.Connect(addr);
  sub.ProcessMessages(message_handler);
  sub.Close();

  return 0;
}
