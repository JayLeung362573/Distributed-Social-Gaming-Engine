#include "Message.h"
#include "GameServer.h"
#include "WebSocketNetworking.h"

#include <thread>
#include <chrono>
#include <set>

std::string formatLobbyList(const std::vector<LobbyInfo>& lobbies) {
    if (lobbies.empty()) {
        return "No existing lobbies";
    }

    std::string output = "Current active lobbies:\n";
    for (const auto& lobby : lobbies) {
        output += " - " + lobby.lobbyName + " (ID: " + lobby.lobbyID + ") | " +
                  "Players: " + std::to_string(lobby.currentPlayers) + "/" +
                  std::to_string(lobby.maxPlayers) + "\n";
    }
    return output;
}

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

        std::set<uintptr_t> knownClients;

        auto last = std::chrono::steady_clock::now();

        while (true) {
            networking->update();

            auto currentClients = networking->getConnectedClientIDs();
            for (uintptr_t clientID : currentClients) {
                if (knownClients.find(clientID) == knownClients.end()) {
                    std::cout << "[Main] New Client Detected: " << clientID << "\n";

                    // 1. Create Fake Browse Request
                    Message browseMsg;
                    browseMsg.type = MessageType::BrowseLobbies;
                    browseMsg.data = BrowseLobbiesMessage{GameType::Default};

                    std::vector<ClientMessage> fakeInput;
                    fakeInput.push_back({clientID, browseMsg});

                    // 2. Get Response from Server
                    auto responses = server->tick(fakeInput);

                    // 3. Process and Send
                    for(auto& response : responses) {
                        if (response.message.type == MessageType::LobbyState) {
                            auto& state = std::get<LobbyStateMessage>(response.message.data);

                            if (state.currentLobbyID.empty()) {
                                std::string text = formatLobbyList(state.lobbies);

                                // Change message type to GameOutput so client prints it
                                response.message.type = MessageType::GameOutput;
                                response.message.data = GameOutputMessage{text};
                            }
                        }

                        networking->sendToClient(response.clientID, response.message);
                    }

                    knownClients.insert(clientID);
                }
            }

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