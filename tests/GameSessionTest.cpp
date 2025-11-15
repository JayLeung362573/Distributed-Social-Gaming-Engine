#include <gtest/gtest.h>
#include "GameSession/GameSession.h"
#include "Lobby/Lobby.h"
#include "GameEngine/Rules.h"

TEST(GameSessionTest, StartRunsRuntimeAndFinishes) {
    // Create dummy rules
    std::vector<std::unique_ptr<ast::Statement>> stmts;
    stmts.push_back(
        ast::makeAssignment(
            ast::makeVariable(Name{"winner"}),
            ast::makeConstant(Value{String{"player1"}})
        )
    );
    GameRules rules{std::span(stmts)};

    // Create dummy players
    std::vector<LobbyMember> players = {
        {1, LobbyRole::Player, true},  // (clientID, role, ready)
        {2, LobbyRole::Player, true}
    };

    GameSession session("testLobby", rules, players);
    session.start();

    EXPECT_TRUE(session.isFinished());
}
