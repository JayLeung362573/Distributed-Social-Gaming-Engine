#include "Message.h"
#include "GameServer.h"
#include "WebSocketNetworking.h"
#include "NetworkManager.h"

#include <thread>
#include <chrono>
#include <set>



int main(int argc, char* argv[])
{
    bool useWebSocket = false;

    if (argc > 1 && std::string(argv[1]) == "--use-websocket") {
        useWebSocket = true;
        std::cout << "Using Web socket network" << "\n";
    }

    if (useWebSocket) {
        auto networking = std::make_shared<WebSocketNetworking>(8080, "../test.html");
        auto server = std::make_shared<GameServer>();
        NetworkManager manager(networking, server);

        networking->startServer();

        auto last = std::chrono::steady_clock::now();

        while (true) {
            networking->update();

            manager.processNewConnections();

            manager.processIncomingMessages();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
    return 0;
}