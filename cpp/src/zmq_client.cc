#include <iostream>
#include <string>

#include "common/application_traits.h"
#include "common/signal_handler.h"

auto main(int argc, char** argv) -> int {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " ADDR." << std::endl;
    return 1;
  }

  std::string addr = argv[1];
  spdlog::info("client connect: {}", addr);

  auto connected_event = [](const zmq_event_t& event, const char* addr) {
    spdlog::info("client connected: addr {}, fd {}", addr, event.value);
  };

  auto message_handler = [](zmq::message_t&& msg) {
    spdlog::info(msg.to_string());
  };

  typename common::ClientTraits::ClientSocket client_one;
  typename common::ClientTraits::ClientSocket client_two;

  client_one.Monitor(connected_event, ZMQ_EVENT_CONNECTED);
  client_one.Connect(addr);
  client_one.ProcessMessages(message_handler);

  client_two.Monitor(connected_event, ZMQ_EVENT_CONNECTED);
  client_two.Connect(addr);
  client_two.ProcessMessages(message_handler);

  client_one.SendMessage("Hello World");
  client_two.SendMessage("I like pizza");

  WaitForSignal();

  client_one.Close();
  client_two.Close();

  return 0;
}
