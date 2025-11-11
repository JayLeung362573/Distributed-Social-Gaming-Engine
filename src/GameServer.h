#pragma once

#include <memory.h>
#include <unordered_map>
#include <vector>
#include "Message.h"
#include "Lobby.h"
#include "LobbyRegistry.h"


class NetworkingInterface;

// Handles game application logic server-side
// Communicates with clients via NetworkingInterface

class GameServer
{
public:
    std::vector<ClientMessage> tick(const std::vector<ClientMessage>& incomingMessages);

private:
    std::vector<ClientMessage> handleClientMessages(const std::vector<ClientMessage>& incomingMessages);

    std::optional<ClientMessage> handleJoinLobbyMessages(const ClientMessage& joinLobbyMsg);
    std::optional<ClientMessage> handleLeaveLobbyMessages(const ClientMessage& leaveMsg);
    std::optional<ClientMessage> handleJoinGameMessages(const ClientMessage& joinGameMsg);
    std::optional<ClientMessage> handleUpdateCycleMessages(const ClientMessage& updateMsg);

    LobbyRegistry m_lobbyRegistry;
};
