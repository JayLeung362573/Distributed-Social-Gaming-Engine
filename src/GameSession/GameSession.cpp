#include "GameSession.h"
#include <iostream>
#include <utility>

GameSession::GameSession(LobbyID lobbyID, ast::GameRules rules, std::vector<LobbyMember> players)
    : m_lobbyID(std::move(lobbyID))
    , m_players(std::move(players))
    , m_runtime(std::make_unique<GameRuntime>(rules))
    {

    std::cout << "[GameSession] Created session for lobby " << m_lobbyID
              << " with " << m_players.size() << " players\n";}

void
GameSession::start() {
    // Ensure m_runtime is available
    if (!m_runtime || m_runtime == nullptr) {
        std::cerr << "[GameSession] No runtime available\n";
        return;
    }

    if(m_runtime->isFinished()){
        std::cout << "[GameSession] Cannot start: already finished or inactive\n";
        return;
    }
    // TODO: simulate simple game without player input first

    // TODO: then use tick() with player input
    try { // employ try/catch wrapping to fast fail bugs 
        m_runtime->run();
    } catch (const std::exception& e) {
        std::cerr << "[GameSession] Runtime error: " << e.what() << "\n";
    }

    std::cout << "[GameSession] Game for lobby " << m_lobbyID << " finished\n";
}

// TODO: Revisit once IO implemented in GameInterpreter
// std::vector<ClientMessage>
// GameSession::tick(const std::vector<ClientMessage>& incomingMessages){
//     if(m_runtime->isFinished()){
//         return {};
//     }

//     std::vector<GameMessage> gameMsgs = extractGameMessages(incomingMessages);

//     auto outGameMsgs = m_runtime->tick(gameMsgs);

//     if(m_runtime->isFinished()){
//         return {};
//     }

//     return convertToClientMessages(outGameMsgs);
// }

std::vector<GameMessage>
GameSession::extractGameMessages(const std::vector<ClientMessage> &clientMsgs) {
    return {};
}

std::vector<ClientMessage>
GameSession::convertToClientMessages(const std::vector<GameMessage> &gameMsgs) {
    return {};
}