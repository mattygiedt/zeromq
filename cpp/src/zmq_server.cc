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

  auto socket_event = [](const zmq_event_t& event, const char* addr) {
    spdlog::info("event type {}, addr {}, fd {}", event.event, addr,
                 event.value);
  };

  typename common::ServerTraits::ServerSocket server;

  server.Monitor(socket_event);
  server.Bind(addr);
  server.ProcessMessages([&server](zmq::message_t&& msg) {
    spdlog::info(msg.to_string());
    auto str = msg.to_string();
    server.SendMessage(str, msg.routing_id());
  });

  server.Close();

  return 0;
}
