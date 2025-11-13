#include "LobbyRegistry.h"
#include <iostream>

Lobby*
LobbyRegistry::createLobby(ClientID hostID, GameType gameType, const std::string &lobbyName) {
    auto lobbyID = generateLobbyID();

    auto lobby = std::make_unique<Lobby>(lobbyID, gameType, hostID, lobbyName);

    lobby->insertPlayer(hostID, LobbyRole::Host);

    auto* lobbyPtr = lobby.get();
    m_lobbies[lobbyID] = std::move(lobby);

    std::cout << "[Registry] Created lobby {" << lobbyName << "} (ID: "<< lobbyID << ")\n";

    return lobbyPtr;
}

std::vector<LobbyInfo>
LobbyRegistry::browseLobbies(GameType gameType) const {
    std::vector<LobbyInfo> result;

    for(const auto& [lobbyID, lobby] : m_lobbies){
        if(lobby->getGameType() == gameType){
            result.push_back(lobby->getInfo());
        }
    }

    std::cout << "[Registry] Found " << result.size() << " lobbies for game type "
              << static_cast<int>(gameType) << "\n";

    return result;
}

Lobby*
LobbyRegistry::joinLobby(ClientID playerID, const LobbyID &lobbyID) {
    auto it = m_lobbies.find(lobbyID);
    if(it == m_lobbies.end()){
        std::cout << "[Registry] Lobby not found: " << lobbyID << "\n";
        return nullptr;
    }

    leaveLobby(playerID); /// prevent player in multiple lobbies

    bool success = it->second->insertPlayer(playerID);
    if(success){
        std::cout << "[Registry] player: " << playerID << " joined Lobby: " << lobbyID << "\n";
        return it->second.get(); /// return the lobby pointer
    }
    return nullptr;
}

bool
LobbyRegistry::destroyLobby(const LobbyID &lobbyID) {
    auto it = m_lobbies.find(lobbyID);
    if(it == m_lobbies.end()){
        return false;
    }
    std::cout << "[Registry] Destroying lobby: " << lobbyID << "\n";
    m_lobbies.erase(it);
    return true;
}

void
LobbyRegistry::leaveLobby(ClientID playerID) {
    auto lobbyIDToLeave = findLobbyForClient(playerID);

    if(!lobbyIDToLeave){
        return;
    }

    auto it = m_lobbies.find(*lobbyIDToLeave);
    auto& lobby = it->second;
    lobby->deletePlayer(playerID);
    std::cout << "[Registry] Player " << playerID << " left lobby: " << *lobbyIDToLeave << "\n";

    if(lobby->getPlayerCount() == 0){
        std::cout << "[Registry] Lobby " << *lobbyIDToLeave << " is empty, destroying\n";
        m_lobbies.erase(it);
    }
}

Lobby *
LobbyRegistry::getLobby(const LobbyID &lobbyID) const {
    auto it = m_lobbies.find(lobbyID);
    if(it != m_lobbies.end()){
        return it->second.get();
    }
    return nullptr;
}

std::optional<LobbyID>
LobbyRegistry::findLobbyForClient(ClientID playerID) const {
    for(const auto& [lobbyID, lobby] : m_lobbies){
        if(lobby->hasPlayer(playerID)){
            return lobbyID;
        }
    }
    return std::nullopt;
}

LobbyID
LobbyRegistry::generateLobbyID() {
    m_lobbyCounter++;
    return "lobby_" + std::to_string(m_lobbyCounter);
}

