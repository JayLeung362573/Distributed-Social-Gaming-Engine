#include <gtest/gtest.h>
#include "parser/GameSpecLoader.h"
#include "parser/GameSpec.h"
#include <filesystem>


class GameSpecLoaderTest : public ::testing::Test {
protected:
    GameSpecLoader loader;
};

TEST_F(GameSpecLoaderTest, LoadGameName) {
    std::filesystem::path filePath = std::filesystem::path(GAMES_DIR) / "hello-test.game";
    GameSpec spec = loader.loadFile(filePath.string().c_str());
    EXPECT_EQ(spec.name, "Hello Test");
}

TEST_F(GameSpecLoaderTest, LoadRockPaperScissors) {
    std::filesystem::path filePath = std::filesystem::path(GAMES_DIR) / "rock-paper-scissors.game";
    GameSpec spec = loader.loadFile(filePath.string().c_str());
    EXPECT_EQ(spec.name, "Rock, Paper, Scissors");
}

TEST_F(GameSpecLoaderTest, LoadFromString) {
    std::string gameText = R"(
configuration {
  name: "Test Game"
  player range: (2, 4)
  audience: false
  setup: { }
}
constants { }
variables { }
per-player { }
per-audience { }
rules { }
)";

    GameSpec spec = loader.loadString(gameText);
    EXPECT_EQ(spec.name, "Test Game");
}

TEST_F(GameSpecLoaderTest, ParseAssignments) {
    // This test will output debug info about parsed assignments
    GameSpec spec = loader.loadFile("../games/test-simple-rules.game");
    EXPECT_EQ(spec.name, "Test Simple Rules");

    // Note: We're just testing that parsing doesn't crash for now
    // Once AST building is implemented, we can test the actual parsed rules
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
