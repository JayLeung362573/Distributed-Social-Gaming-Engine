#include "Message.h"
#include "Networking.h"
#include "GameClient.h"
#include "GameServer.h"

#include "WebSocketNetworking.h"
#include "MessageTranslator.h"

#include <thread>
#include <chrono>

// simple demo of two clients sending a JoinGame message to the server
int main(int argc, char* argv[])
{
    bool useWebSocket = false;

    if (argc > 1 && std::string(argv[1]) == "--use-websocket") {
        useWebSocket = true;
        std::cout << "Using Web socket network" << "\n";
    }

    if (useWebSocket) {
        auto networking = std::make_shared<WebSocketNetworking>(8080, "../test.html");
        auto server = std::make_unique<GameServer>();

        networking->startServer();

        int cycle = 1;
        auto last = std::chrono::steady_clock::now();

        while (true) {
            networking->update();

            // pass incoming network messages to game server
            auto incomingMessages = networking->receiveFromClients();

            std::vector<ClientMessage> clientMessages;
            for(auto& [clientID, message] : incomingMessages){
                std::cout << "[Network] Processing incoming messages" << '\n';
                clientMessages.push_back({clientID, message});
            }

            // process game logic in game server
            auto outgoingMessages = server->tick(clientMessages);

            // send outgoing processed gameServer messages
            for(const auto& clientMsg : outgoingMessages){
                networking->sendToClient(clientMsg.clientID, clientMsg.message);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
    return 0;
}