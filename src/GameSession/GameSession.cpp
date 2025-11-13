#include "GameSession.h"
#include <iostream>
#include <utility>

GameSession::GameSession(std::string gameID, GameRules rules, std::vector<LobbyMember> players)
    : m_gameID(std::move(gameID))
    , m_runtime(std::make_unique<GameRuntime>(rules))
    , m_active(true), m_finished(false){

    std::cout << "[GameSession] Created session " << m_gameID << '\n';
}

void
GameSession::start() {
    if(m_finished || !m_active){
        std::cout << "[GameSession] Cannot start: already finished or inactive\n";
        return;
    }
    // TODO: simulate simple game without player input first

    // TODO: then use tick() with player input
    m_runtime->run();
}