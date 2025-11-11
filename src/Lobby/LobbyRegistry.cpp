#pragma once
#include "LobbyRegistry.h"
#include <iostream>

LobbyID
LobbyRegistry::createLobby(ClientID hostID, GameType gameType, const std::string &lobbyName) {
    auto lobbyID = generateLobbyID();

    auto lobby = std::make_unique<Lobby>(lobbyID, gameType, hostID, lobbyName);
    m_lobbies[lobbyID] = std::move(lobby);

    std::cout << "[Registry] Created lobby {" << lobbyName << "} (ID: "<< lobbyID << ")\n";

    return lobbyID;
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

bool
LobbyRegistry::joinLobby(ClientID playerID, const LobbyID &lobbyID) {
    auto it = m_lobbies.find(lobbyID);
    if(it == m_lobbies.end()){
        std::cout << "[Registry] Lobby not found: " << lobbyID << "\n";
        return false;
    }

    std::cout << "player: " << playerID << " joined Lobby: " << lobbyID << "\n";
    return it->second->insertPlayer(playerID);
}

bool
LobbyRegistry::destroyLobby(const LobbyID &lobbyID) {
    auto it = m_lobbies.find(lobbyID);
    if(it != m_lobbies.end()){
        return false;
    }
    std::cout << "[Registry] Destroying lobby: " << lobbyID << "\n";
    m_lobbies.erase(it);
    return true;
}

void
LobbyRegistry::leaveLobby(ClientID playerID) {
    for(auto& [lobbyID, lobby] : m_lobbies){
        if(lobby->hasPlayer(playerID)){
            lobby->deletePlayer(playerID);
            std::cout << "[Registry] Player " << playerID << " left lobby: " << lobbyID << "\n";

            if(lobby->getPlayerCount() == 0){
                std::cout << "[Registry] Lobby " << lobbyID << " is empty, destroying\n";
                destroyLobby(lobbyID);
            }
        }
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
    size_t counter = 0;
    counter++;
    return "lobby_" + std::to_string(counter);
}

