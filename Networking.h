#pragma once

#include <unordered_map>
#include <memory>
#include "Message.h"

class GameServer;
class GameClient;

class NetworkingInterface
{
    public:
        virtual void sendMessageToClient(int toClientID, Message &message) = 0;
        virtual void sendMessageToServer(int fromClientID, Message &message) = 0;

};

class InMemoryNetworking : public NetworkingInterface
{
    public:
        void setServer(std::shared_ptr<GameServer> server);
        void addClient(std::shared_ptr<GameClient> client);

        void sendMessageToClient(int toClientID, Message &message) override;
        void sendMessageToServer(int fromClientID, Message &message) override;

    private:
        std::unordered_map<int, std::shared_ptr<GameClient>> m_clients;
        std::shared_ptr<GameServer> m_server;
};

// TODO (if interface makes sense):
// class WebSocketNetworking : public NetworkingInterface {}
