#include <iostream>
#include "GameServer.h"
#include "Message.h"

GameServer::GameServer()
    : m_gameStarted(false), m_gameOver(false){}

std::vector<ClientMessage>
GameServer::tick(const std::vector<ClientMessage> &incomingMessages) {
    if(getState() == ServerState::InLobby){
        return handleLobbyMessages(incomingMessages);
    }else if(getState() == ServerState::InGame){
        return handleGameMessages(incomingMessages);
    }
    return {};
}

std::vector<ClientMessage>
GameServer::handleLobbyMessages(const std::vector<ClientMessage> &incomingMessages) {
    std::vector<ClientMessage> outgoingMessages;

    for(const auto& clientMsg : incomingMessages){
        switch(clientMsg.message.type)
        {
            case MessageType::JoinLobby:
            {
                auto &joinData = std::get<JoinLobbyMessage>(clientMsg.message.data);
                std::cout << "[GameServer] Player {" << joinData.playerName
                          << "} attempting to join lobby (clientID = " << clientMsg.clientID << ")\n";

                LobbyID lobbyID;

                if(joinData.lobbyName.empty()){
                    /// create new lobby for this player
                    lobbyID = m_lobbyRegistry.createLobby(
                            clientMsg.clientID,
                            GameType::Default,
                            joinData.playerName + "'s Lobby"
                            );
                    m_lobbyRegistry.movePlayerToLobby(clientMsg.clientID, lobbyID);
                    std::cout << "[GameServer] Created and joined lobby: " << lobbyID << "\n";
                }else{
                    /// join an existing lobby
                    lobbyID = joinData.lobbyName;
                    bool joined = m_lobbyRegistry.movePlayerToLobby(clientMsg.clientID, lobbyID);

                    if(!joined){
                        std::cout << "[GameServer] Failed to join lobby: " << lobbyID << "\n";

                        Message errorMsg;
                        errorMsg.type = MessageType::Empty;
                        outgoingMessages.push_back({clientMsg.clientID, errorMsg});
                        continue;
                    }
                    std::cout << "[GameServer] Joined existing lobby: " << lobbyID << "\n";
                }

                Message response;
                response.type = MessageType::JoinLobby;
                response.data = JoinLobbyMessage{joinData.playerName};
                outgoingMessages.push_back({clientMsg.clientID, response});

                auto lobbyStateMessages = broadcastLobbyStateToLobby(lobbyID);
                outgoingMessages.insert(
                        outgoingMessages.end(),
                        lobbyStateMessages.begin(),
                        lobbyStateMessages.end()
                );
                break;
            }
            case MessageType::LeaveLobby:
            {
                auto &leaveData = std::get<LeaveLobbyMessage>(clientMsg.message.data);
                std::cout << "[GameServer] Player {" << leaveData.playerName
                          << "} leaving lobby (clientID = " << clientMsg.clientID << ")\n";

                auto maybeLobbyID = m_lobbyRegistry.findLobbyForClient(clientMsg.clientID);
                if(!maybeLobbyID){
                    std::cout << "[GameServer] Warning: Player not in any lobby\n";
                    break;
                }

                LobbyID lobbyID = *maybeLobbyID;
                m_lobbyRegistry.removePlayerFromAllLobbies(clientMsg.clientID);
                std::cout << "[GameServer] Player left lobby: " << lobbyID << "\n";

                Message response;
                response.type = MessageType::LeaveLobby;
                response.data = LeaveLobbyMessage{leaveData.playerName};
                outgoingMessages.push_back({clientMsg.clientID, response});

                auto lobbyStateMessages = broadcastLobbyStateToLobby(lobbyID);
                outgoingMessages.insert(
                        outgoingMessages.end(),
                        lobbyStateMessages.begin(),
                        lobbyStateMessages.end()
                );
                break;
            }
            case MessageType::JoinGame:
                break;
        }
    }
    return outgoingMessages;
}

std::vector<ClientMessage>
GameServer::handleGameMessages(const std::vector<ClientMessage> &incomingMessages) {
    std::vector<ClientMessage> outgoingMessages;

    for(const auto& clientMsg : incomingMessages){
        switch (clientMsg.message.type)
        {
            case MessageType::JoinGame:
            {
                auto& joinData = std::get<JoinGameMessage>(clientMsg.message.data);
                std::cout << "JoinGame(\"" << joinData.playerName << "\")\n";

                Message response;
                response.type = MessageType::JoinGame;
                response.data = JoinGameMessage{joinData.playerName};

                outgoingMessages.push_back({clientMsg.clientID, response});
                break;
            }
            case MessageType::UpdateCycle:
            {
                auto& data = std::get<UpdateCycleMessage>(clientMsg.message.data);
                std::cout << "UpdateCycle from client with cycle " << data.cycle << "\n";
                break;
            }
            default:
            {
                std::cout << "Empty message" << "\n";
                break;
            }
        }
    }

    // TODO: process game logic with GameEngine

    return outgoingMessages;
}

GameServer::ServerState
GameServer::getState() const {
    if(m_gameStarted) return ServerState::InGame;
    if(m_gameOver) return ServerState::GameOver;
    return ServerState::InLobby;
}

std::vector<ClientMessage>
GameServer::broadcastLobbyStateToLobby(const LobbyID& lobbyID) {
    std::vector<ClientMessage> messages;

    const Lobby* lobby = m_lobbyRegistry.getLobby(lobbyID);
    if(!lobby){
        std::cout << "[GameServer] Warning: Cannot broadcast - lobby not found: "
                  << lobbyID << "\n";
        return messages;
    }

    auto players = lobby->getAllPlayer();

    std::vector<LobbyInfo> thisLobbyInfo = {lobby->getInfo()};

    Message msg;
    msg.type = MessageType::LobbyState;
    msg.data = LobbyStateMessage{thisLobbyInfo, lobbyID};

    for (const auto& player : players) {
        messages.push_back({player.clientID, msg});
    }

    std::cout << "[GameServer] Broadcasting lobby state to " << players.size()
              << " players in lobby " << lobbyID << "\n";

    return messages;
}