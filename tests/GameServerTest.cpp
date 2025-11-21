#include <gtest/gtest.h>
#include "GameServer.h"
#include "Message.h"

TEST(GameServerTest, SingleClientJoinGame){
    GameServer server;

    std::vector<ClientMessage> incoming = {
            {1, {MessageType::StartGame, StartGameMessage{"player 1"}}}
    };

    std::vector<ClientMessage> outgoingMsg = server.tick(incoming);

    ASSERT_GT(outgoingMsg.size(), 0);
    ASSERT_EQ(outgoingMsg[0].clientID, 1);
    ASSERT_EQ(outgoingMsg[0].message.type, MessageType::StartGame);

    auto& firstResponse = outgoingMsg[0];
    auto& responseMsg = std::get<StartGameMessage>(firstResponse.message.data);
    ASSERT_EQ(responseMsg.playerName, "player 1");
}

TEST(GameServerTest, MultipleClientsJoinGame){
    GameServer server;

    std::vector<ClientMessage> incoming = {
            {1, {MessageType::StartGame, StartGameMessage{"player 1"}}},
            {2, {MessageType::StartGame, StartGameMessage{"player 2"}}},
            {3, {MessageType::StartGame, StartGameMessage{"player 3"}}},

    };

    std::vector<ClientMessage> outgoingMsg = server.tick(incoming);

    ASSERT_EQ(outgoingMsg.size(), 3);
    bool foundClient1 = false, foundClient2 = false, foundClient3 = false;
    for(const auto& msg : outgoingMsg){
        if(msg.clientID == 1) foundClient1 = true;
        if(msg.clientID == 2) foundClient2 = true;
        if(msg.clientID == 3) foundClient3 = true;
    }
    ASSERT_TRUE(foundClient1);
    ASSERT_TRUE(foundClient2);
    ASSERT_TRUE(foundClient3);
}

// michellenous (integrated) tests
TEST(GameServerTest, ClientNotInLobbyCannotStart) {
    GameServer server;
    uintptr_t clientID = 5;
    StartGameMessage msg{"Player_1"};
    auto res = server.handleStartGameMessages(clientID, msg);

    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].message.type, MessageType::Error);
    EXPECT_EQ(std::get<ErrorMessage>(res[0].message.data).reason, "You must be in a lobby to start a game");
}

TEST(GameServerTest, LobbyRemovedCausesError) {
    GameServer server;
    uintptr_t hostID = 1;
    JoinLobbyMessage join{"host", "Lobby_1"};
    server.handleJoinLobbyMessages(hostID, join);

    // forcibly remove lobby (simulate missing registry entry)
    auto registry = server.getLobbyRegistryForTest(); 
    registry->clear(); // forcefully remove the registry

    StartGameMessage start{"host"};
    auto res = server.handleStartGameMessages(hostID, start);
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0].message.type, MessageType::Error);
    EXPECT_EQ(std::get<ErrorMessage>(res[0].message.data).reason, "Lobby not found");
}

TEST(GameServerTest, MultipleActiveSessionsTracked) {
    GameServer server;

    uintptr_t h1 = 1, h2 = 2;
    server.handleJoinLobbyMessages(h1, {"host_1", "Lobby1"});
    server.handleJoinLobbyMessages(h2, {"host_2", "Lobby2"});

    auto r1 = server.handleStartGameMessages(h1, {"host_1"});
    auto r2 = server.handleStartGameMessages(h2, {"host_2"});

    EXPECT_FALSE(r1.empty());
    EXPECT_FALSE(r2.empty());
    EXPECT_GE(server.getActiveSessionCountForTest(), 2);
}

