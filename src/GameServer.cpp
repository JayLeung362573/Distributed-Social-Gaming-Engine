#include <iostream>
#include "GameServer.h"
#include "Message.h"

std::vector<ClientMessage>
GameServer::tick(const std::vector<ClientMessage> &incomingMessages) {
    return handleClientMessages(incomingMessages);
}

std::vector<ClientMessage>
GameServer::handleClientMessages(const std::vector<ClientMessage> &incomingMessages) {
    std::vector<ClientMessage> outgoingMessages;

    for(const auto& clientMsg : incomingMessages){
        std::optional<ClientMessage> response;

        switch(clientMsg.message.type)
        {
            case MessageType::Empty:
                break;
            case MessageType::JoinGame:
                response = handleJoinGameMessages(clientMsg);
                break;
            case MessageType::UpdateCycle:
                response = handleUpdateCycleMessages(clientMsg);
                break;
            case MessageType::JoinLobby:
                response = handleJoinLobbyMessages(clientMsg);
                break;
            case MessageType::LeaveLobby:
                response = handleLeaveLobbyMessages(clientMsg);
                break;
            case MessageType::LobbyState:
                break;
            case MessageType::BrowseLobbies:
                break;
            case MessageType::GetLobbyState:
                break;
            default:
                std::cout << "Empty message\n";
                break;
        }
        if(response){
            outgoingMessages.push_back(*response);
        }
    }
    return outgoingMessages;
}

std::optional<ClientMessage>
GameServer::handleJoinLobbyMessages(const ClientMessage& joinLobbyMsg) {
    auto &joinData = std::get<JoinLobbyMessage>(joinLobbyMsg.message.data);
    std::cout << "[GameServer] Player {" << joinData.playerName
              << "} attempting to join lobby (clientID = " << joinLobbyMsg.clientID << ")\n";

    LobbyID lobbyID;

    if(joinData.lobbyName.empty()){
        lobbyID = m_lobbyRegistry.createLobby(
                joinLobbyMsg.clientID,
                GameType::Default,
                joinData.playerName + "'s Lobby"
        );
    } else {
        lobbyID = joinData.lobbyName;
        bool joined = m_lobbyRegistry.joinLobby(joinLobbyMsg.clientID, lobbyID);

        if(!joined){
            Message errorMsg;
            errorMsg.type = MessageType::Empty;
            return ClientMessage{joinLobbyMsg.clientID, errorMsg};
        }
    }

    Message response;
    response.type = MessageType::JoinLobby;
    response.data = JoinLobbyMessage{joinData.playerName, lobbyID};
    return ClientMessage{joinLobbyMsg.clientID, response};
}

std::optional<ClientMessage>
GameServer::handleLeaveLobbyMessages(const ClientMessage& leaveMsg)
{
    auto& leaveData = std::get<LeaveLobbyMessage>(leaveMsg.message.data);
    std::cout << "[GameServer] Player '" << leaveData.playerName << "' leaving lobby\n";

    m_lobbyRegistry.leaveLobby(leaveMsg.clientID);

    Message response;
    response.type = MessageType::LeaveLobby;
    response.data = LeaveLobbyMessage{leaveData.playerName};
    return ClientMessage{leaveMsg.clientID, response};
}


std::optional<ClientMessage>
GameServer::handleJoinGameMessages(const ClientMessage& joinGameMsg){
    auto& joinData = std::get<JoinGameMessage>(joinGameMsg.message.data);
    std::cout << "[GameServer] JoinGame: " << joinData.playerName << "\n";

    Message response;
    response.type = MessageType::JoinGame;
    response.data = JoinGameMessage{joinData.playerName};
    return ClientMessage{joinGameMsg.clientID, response};
}

std::optional<ClientMessage>
GameServer::handleUpdateCycleMessages(const ClientMessage& updateMsg) {
    auto& data = std::get<UpdateCycleMessage>(updateMsg.message.data);
    std::cout << "[GameServer] UpdateCycle: " << data.cycle << "\n";

    // No response needed
    return std::nullopt;
}

