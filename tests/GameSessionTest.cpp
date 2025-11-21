#include <gtest/gtest.h>
#include "GameSession/GameSession.h"
#include "Lobby/Lobby.h"
#include "GameEngine/Rules.h"

using namespace ast;

TEST(GameSessionTest, StartRunsRuntimeAndFinishes) {
    // Create dummy rules
    std::vector<std::unique_ptr<Statement>> stmts;
    stmts.push_back(
        makeAssignment(
            makeVariable(Name{"winner"}),
            makeConstant(Value{String{"player1"}})
        )
    );
    GameRules rules{std::move(stmts)};

    // Create dummy players
    std::vector<LobbyMember> players = {
        {1, LobbyRole::Player, true}
    };

    GameSession session("lobby_1", std::move(rules), players);
    session.start();

    EXPECT_TRUE(session.isFinished());
}

TEST(GameSessionTest, StartTwiceDoesNotCrash) {
    std::vector<std::unique_ptr<Statement>> stmts;
    stmts.push_back(makeAssignment(makeVariable(Name{"winner"}), makeConstant(Value{String{"A"}})));
    GameRules rules{std::move(stmts)};

    std::vector<LobbyMember> players = {{1, LobbyRole::Player, true}};
    GameSession session("lobby_test", std::move(rules), players);

    EXPECT_NO_THROW(session.start());
    EXPECT_TRUE(session.isFinished());
    EXPECT_NO_THROW(session.start()); // should just log a message, not throw
}

TEST(GameSessionTest, ExtractAndConvertAreEmpty) {
    GameRules rules{};
    std::vector<LobbyMember> players;
    GameSession session("lobby_test", std::move(rules), players);

    std::vector<ClientMessage> dummy;
    EXPECT_TRUE(session.extractGameMessages(dummy).empty());
    EXPECT_TRUE(session.convertToClientMessages({}).empty());
}

// TODO: Revisit once IO implemented in GameInterpreter
// TEST(GameSessionTest, TickReturnsEmptyWhenFinished) {
//     // Single statement game that finishes immediately
//     std::vector<std::unique_ptr<ast::Statement>> stmts;
//     stmts.push_back(
//         ast::makeAssignment(
//             ast::makeVariable(Name{"score"}),
//             ast::makeConstant(Value{Integer{10}})
//         )
//     );

//     ast::GameRules rules{std::move(stmts)};
//     std::vector<LobbyMember> players = {
//         {1, LobbyRole::Player, true}
//     };

//     GameSession session("lobby_2", rules, players);
//     session.start(); // Completes game

//     std::vector<ClientMessage> dummy;
//     auto out = session.tick(dummy);
//     EXPECT_TRUE(out.empty());
// }