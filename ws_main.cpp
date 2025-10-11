#include "WebSocket.h"
#include "GameServer.h"

int main() {
  auto net = std::make_shared<WebSocket>(5001, "../lib/html/webchat.html");
  auto server = std::make_shared<GameServer>(net);
  net->setServer(server);

  while (true) {
    net->update();
  }

  return 0;
}
