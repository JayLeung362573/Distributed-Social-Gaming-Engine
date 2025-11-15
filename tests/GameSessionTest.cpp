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
        {1, LobbyRole::Player, true}    
    };

    GameSession session("lobby_1", rules, players);
    session.start();

    EXPECT_TRUE(session.isFinished());
}

TEST(GameSessionTest, TickReturnsEmptyWhenFinished) {
    // Single statement game that finishes immediately
    std::vector<std::unique_ptr<ast::Statement>> stmts;
    stmts.push_back(
        ast::makeAssignment(
            ast::makeVariable(Name{"score"}),
            ast::makeConstant(Value{Integer{10}})
        )
    );

    GameRules rules{std::span(stmts)};
    std::vector<LobbyMember> players = {
        {1, LobbyRole::Player, true}    
    };

    GameSession session("lobby_2", rules, players);
    session.start(); // Completes game

    std::vector<ClientMessage> dummy;
    auto out = session.tick(dummy);
    EXPECT_TRUE(out.empty());
}