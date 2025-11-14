#include <string>
#include <vector>
#include <memory>
#include "Message.h"
#include "Lobby/Lobby.h"
#include "GameEngine/Rules.h"
#include "GameEngine/GameRuntime.h"

class GameSession{
public:
    GameSession(LobbyID lobbyID,
                GameRules rules,
                std::vector<LobbyMember> players);

    void start();

    bool isActive() const {return m_active;}
    bool isFinished() const {return m_finished;}
    LobbyID getLobbyID() const {return m_lobbyID;}
    std::vector<ClientMessage> getResultMessages() const;

private:
    LobbyID m_lobbyID;
    std::vector<LobbyMember> m_players;
    std::unique_ptr<GameRuntime> m_runtime;
    bool m_active = false;
    bool m_finished = false;
};