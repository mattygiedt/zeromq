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

  typename common::RadioTraits::RadioSocket radio;

  radio.Connect(addr);

  for (int i = 0; i < common::CommonTraits::kLoop; ++i) {
    std::this_thread::sleep_for(common::CommonTraits::kLoopWait);
    radio.SendMessage("Hello World: " + std::to_string(i),
                      common::RadioTraits::kGroup);
  }

  radio.Close();

  return 0;
}
