#include "GameSession.h"
#include <iostream>
#include <utility>

GameSession::GameSession(LobbyID lobbyID, GameRules rules, std::vector<LobbyMember> players)
    : m_lobbyID(std::move(lobbyID))
    , m_players(std::move(players))
    , m_runtime(std::make_unique<GameRuntime>(rules))
    , m_finished(false){

    std::cout << "[GameSession] Created session for lobby " << m_lobbyID
              << " with " << m_players.size() << " players\n";}

void
GameSession::start() {
    if(m_finished){
        std::cout << "[GameSession] Cannot start: already finished or inactive\n";
        return;
    }
    // TODO: simulate simple game without player input first

    // TODO: then use tick() with player input
    m_runtime->run();

    m_finished = true;
    std::cout << "[GameSession] Game for lobby " << m_lobbyID << " finished\n";
}