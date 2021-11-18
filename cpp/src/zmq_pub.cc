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

  typename common::PubTraits::PubSocket pub;

  pub.Monitor(socket_event);
  pub.Bind(addr);

  for (auto i = 0; i < common::CommonTraits::kLoop; ++i) {
    std::this_thread::sleep_for(common::CommonTraits::kLoopWait);
    if (i % 2 == 0) {
      // zmq::send_flags::none
      pub.SendMessage("Hello World: " + std::to_string(i));
    } else {
      // zmq::send_flags::sndmore
      pub.SendMessage("Hello World: " + std::to_string(i), true);
    }
  }

  pub.Close();

  return 0;
}
