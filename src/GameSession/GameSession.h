#include <string>
#include <vector>
#include <memory>
#include "Message.h"
#include "Lobby/Lobby.h"
#include "GameEngine/Rules.h"
#include "GameEngine/GameRuntime.h"

class GameSession{
public:
    GameSession(std::string gameID,
                GameRules rules,
                std::vector<LobbyMember> players);

    void start();

    bool isActive() const {return m_active;}
    bool isFinished() const {return m_finished;}
    std::string getGameID() const {return m_gameID;};

private:
    std::string m_gameID;
    std::unique_ptr<GameRuntime> m_runtime;
    bool m_active = false;
    bool m_finished = false;
};