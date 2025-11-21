#include <gtest/gtest.h>
#include "MessageTranslator.h"

TEST(MessageTranslatorTest, StartGameSerializeDeserialize) {
    StartGameMessage msg{"player_1"};
    Message original{MessageType::StartGame, msg};

    auto serialized = MessageTranslator::serialize(original);
    EXPECT_TRUE(serialized.starts_with("JoinGame:") || serialized.starts_with("StartGame:"));

    auto deserialized = MessageTranslator::deserialize(serialized);
    EXPECT_EQ(deserialized.type, MessageType::StartGame);
    EXPECT_EQ(std::get<StartGameMessage>(deserialized.data).playerName, "player_1");
}