# zeromq
A quick tour of zeromq's different  DRAFT APIs socket types

## Read This First
This repo is basically just taking the excellent content found [here](https://brettviren.github.io/cppzmq-tour/index.html?amp=1), and generating stupid simple applications that show how to use each socket type.

## Building
Project uses VSCode `.devcontainer` support -- look in the directory for a sample `Dockerfile` if you want to roll your own.
```
git clone git@github.com:mattygiedt/zeromq.git
cd zeromq
mkdir build
cd build
cmake ..
make -j4
```

### Client / Server
ZMQ solves all the problems. Not sure why you'd use any other type of socket for general client / server interaction, where you define your own RPC message sematics within the message payload. These sockets are thread safe, and monitorable.

Here's the ubiquitous echo example, where the client send a string `"Hello World"` which gets immediately reflected back to it. Try connecting a bunch of clients. Try bringing the server up/down as you do this. It can't get any easier!
```
  // zmq_client.cc

  auto connected_event = [](const zmq_event_t& event, const char* addr) {
    spdlog::info("client connected: addr {}, fd {}", addr, event.value);
  };

  auto message_handler = [](zmq::message_t&& msg) {
    spdlog::info(msg.to_string());
  };

  typename common::ClientTraits::ClientSocket client;

  client.Monitor(connected_event, ZMQ_EVENT_CONNECTED);
  client.Connect(addr);
  client.SendMessage("Hello World");
  client.ProcessMessages(message_handler);
  client.Close();
```
```
  // zmq_server.cc

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
```

### Radio / Dish
ZMQ does multicast. Technically, I think there is / was support for transmitting UDP previous to these socket types, but not 100 delta. Anyway, here's a working example!

The `radio` socket publishes multicast packets to a given group, defined as a `char *`. This is similar to the `PUB/SUB` but less wonky than using message parts to denote envelopes. The group defined here is `"//eqty/aapl"`, which both the radio and dish use. Radio publishes `Hello World` plus the `for_loop` counter value so you can connect multiple dish sockets and watch the magic happen. The included address `"udp://239.192.0.1:7500"` works for me, might need to change for you; I am many things, but a network admin is not one of them.
```
  // zmq_radio.cc
  typename common::RadioTraits::RadioSocket radio;

  radio.Connect(addr);

  for (int i = 0; i < common::CommonTraits::kLoop; ++i) {
    std::this_thread::sleep_for(common::CommonTraits::kLoopWait);
    radio.SendMessage("Hello World: " + std::to_string(i),
                      common::RadioTraits::kGroup);
  }

  radio.Close();
```
```
  // zmq_dish.cc
  auto message_handler = [](zmq::message_t&& msg) {
    spdlog::info(msg.to_string());
  };

  typename common::DishTraits::DishSocket dish;

  dish.Join(common::RadioTraits::kGroup);
  dish.Bind(addr);
  dish.ProcessMessages(message_handler);
  dish.Close();
```

### Pub / Sub / Push / Pull / Pair
All your favorites are included because why not. Actually it's interesting to compare / contrast the new sockets against the battle-tested work horses.
