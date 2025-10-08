#pragma once

#include <memory.h>
#include <unordered_map>
#include "Message.h"


class NetworkingInterface;

class GameServer
{
    public:
        GameServer(std::shared_ptr<NetworkingInterface> networking);
        void onMessageFromClient(int fromClientID, Message &message);

    private:
        std::shared_ptr<NetworkingInterface> m_networking;
};
