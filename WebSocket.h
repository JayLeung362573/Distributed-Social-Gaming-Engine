#pragma once
#include "Networking.h"
#include "Server.h"
#include "Message.h"

#include <memory>
#include <unordered_map>
#include <vector>

class GameServer;

class WebSocket : public NetworkingInterface {
public:
    WebSocket(unsigned short port, const std::string& htmlPath);
    void setServer(std::shared_ptr<GameServer> server);
    void update();

    void sendMessageToClient(int toClientID, Message &message) override;
    void sendMessageToServer(int fromClientID, Message &message) override;

private:
    std::shared_ptr<GameServer> gameServer;
    std::unique_ptr<networking::Server> wsServer;
    std::vector<networking::Connection> connections;
    std::unordered_map<int, std::string> players;

    void handleConnect(networking::Connection c);
    void handleDisconnect(networking::Connection c);
};