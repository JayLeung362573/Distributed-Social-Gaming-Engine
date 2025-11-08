#include <gtest/gtest.h>
#include "Lobby.h"
#include "LobbyRegistry.h"

// Individual Lobby Tests
TEST(LobbyTest, CreateEmptyLobby) {
    Lobby lobby("lobby_0", GameType::Default, 1, "EmptyLobby"); // Create lobby, assign player with ClientID 1 as host
    EXPECT_FALSE(lobby.hasPlayer(2));
    EXPECT_TRUE(lobby.hasPlayer(1));
    EXPECT_TRUE(lobby.getHostID() == 1);
    EXPECT_EQ(lobby.getPlayerCount(), 1);
}

TEST(LobbyTest, MultiplePlayerLobby) {
    Lobby lobby("lobby_1", GameType::Default, 1, "TestLobby"); // Create lobby, assign player with ClientID 1 as host
    EXPECT_FALSE(lobby.hasPlayer(2));
    EXPECT_TRUE(lobby.hasPlayer(1));

    bool inserted = lobby.insertPlayer(2, LobbyRole::Player);
    EXPECT_TRUE(inserted);
    EXPECT_TRUE(lobby.hasPlayer(2));

    EXPECT_EQ(lobby.getPlayerCount(), 2);
}

TEST(LobbyTest, PreventDuplicateInsert) {
    Lobby lobby("lobby_2", GameType::Default, 1, "DuplicateLobby");
    EXPECT_TRUE(lobby.insertPlayer(2));
    EXPECT_FALSE(lobby.insertPlayer(2)); // Duplicate player should not be inserted
    EXPECT_EQ(lobby.getPlayerCount(), 2); // Should remain the same non-duplicated size
}

TEST(LobbyTest, DeletePlayerRemovesThem) {
    Lobby lobby("lobby_3", GameType::Default, 1, "DeleteTest");
    lobby.insertPlayer(2);
    EXPECT_TRUE(lobby.hasPlayer(2));
    EXPECT_EQ(lobby.getPlayerCount(), 2);

    lobby.deletePlayer(2);
    EXPECT_FALSE(lobby.hasPlayer(2));
    EXPECT_EQ(lobby.getPlayerCount(), 1);
}

TEST(LobbyTest, IsFullPreventsInsert) {
    Lobby lobby("lobby_4", GameType::Default, 1, "FullLobby");

    // Fill up to maxPlayers (10)
    size_t max_players_size = lobby.getInfo().maxPlayers; // Maybe have a getter that access the max size directly (it seems like a reusable variable)

    for (int i = 0; i < max_players_size; ++i) {
        EXPECT_TRUE(lobby.insertPlayer(100 + i));
    }

    EXPECT_TRUE(lobby.isFull());
    EXPECT_EQ(lobby.getPlayerCount(), max_players_size); // plus the host
    EXPECT_FALSE(lobby.insertPlayer(max_players_size * 100 - 1)); // Cannot insert into full lobby
}

// LobbyRegistry Tests
TEST(LobbyRegistryTest, JoinEmptyLobbyCreatesNew) {
    LobbyRegistry registry;

    LobbyID lobbyID = registry.createLobby(1, GameType::Default, "Bob's Game");
    EXPECT_FALSE(lobbyID.empty());

    const Lobby* lobby = registry.getLobby(lobbyID);
    ASSERT_NE(lobby, nullptr);
    EXPECT_EQ(lobby->getInfo().lobbyName, "Bob's Game");
}

TEST(LobbyRegistryTest, MovePlayerToExistingLobby) {
    LobbyRegistry registry;
    LobbyID id = registry.createLobby(1, GameType::Default, "Chess Match");

    bool joined = registry.movePlayerToLobby(2, id);
    EXPECT_TRUE(joined);

    const Lobby* lobby = registry.getLobby(id);
    ASSERT_NE(lobby, nullptr);
    EXPECT_TRUE(lobby->hasPlayer(2));
}

TEST(LobbyRegistryTest, MovePlayerToNonExistentLobbyFails) {
    LobbyRegistry registry;
    bool joined = registry.movePlayerToLobby(1, "invalid_lobby_id");
    EXPECT_FALSE(joined);
}

TEST(LobbyRegistryTest, RemovePlayerFromAllLobbies) {
    LobbyRegistry registry;
    LobbyID id1 = registry.createLobby(1, GameType::Default, "Test1");
    LobbyID id2 = registry.createLobby(2, GameType::Default, "Test2");

    registry.movePlayerToLobby(3, id1);
    registry.movePlayerToLobby(3, id2);

    registry.removePlayerFromAllLobbies(3);

    EXPECT_FALSE(registry.getLobby(id1)->hasPlayer(3));
    EXPECT_FALSE(registry.getLobby(id2)->hasPlayer(3));
}

TEST(LobbyRegistryTest, FindLobbyForClientReturnsCorrectLobby) {
    LobbyRegistry registry;
    LobbyID id1 = registry.createLobby(1, GameType::Default, "FindTest");

    registry.movePlayerToLobby(5, id1);
    auto result = registry.findLobbyForClient(5);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), id1);
}

TEST(LobbyRegistryTest, FindLobbyForNonexistentClientReturnsNullopt) {
    LobbyRegistry registry;
    LobbyID id1 = registry.createLobby(1, GameType::Default, "FindNone");

    auto result = registry.findLobbyForClient(99);
    EXPECT_FALSE(result.has_value());
}
