#include <gtest/gtest.h>
#include <string_view>
#include <vector>

#include "Network/MessageTranslator.h"
#include "Message.h"

namespace {

void expectMalformed(std::string_view payload) {
    Message msg = MessageTranslator::deserialize(payload);

    EXPECT_EQ(msg.type, MessageType::Empty)
        << "Expected malformed payload to be rejected: " << payload;

    EXPECT_TRUE(std::holds_alternative<std::monostate>(msg.data))
        << "Malformed payload should contain monostate: " << payload;
}

}

TEST(MessageTranslatorTest, DeserializesStartGameMessage) {
    Message msg = MessageTranslator::deserialize("StartGame:Alice");

    EXPECT_EQ(msg.type, MessageType::StartGame);

    const auto* data = std::get_if<StartGameMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->playerName, "Alice");
}

TEST(MessageTranslatorTest, DeserializesJoinLobbyMessage) {
    Message msg = MessageTranslator::deserialize("InternalJoinLobbyAlice|Lobby1|0");

    EXPECT_EQ(msg.type, MessageType::JoinLobby);

    const auto* data = std::get_if<JoinLobbyMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->playerName, "Alice");
    EXPECT_EQ(data->lobbyName, "Lobby1");
    EXPECT_EQ(data->gameType, 0);
}

TEST(MessageTranslatorTest, DeserializesLeaveLobbyMessage) {
    Message msg = MessageTranslator::deserialize("LeaveLobbyAlice");

    EXPECT_EQ(msg.type, MessageType::LeaveLobby);

    const auto* data = std::get_if<LeaveLobbyMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->playerName, "Alice");
}

TEST(MessageTranslatorTest, DeserializesBrowseLobbiesMessage) {
    Message msg = MessageTranslator::deserialize("BrowseLobbies0");

    EXPECT_EQ(msg.type, MessageType::BrowseLobbies);

    const auto* data = std::get_if<BrowseLobbiesMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
}

TEST(MessageTranslatorTest, DeserializesResponseTextInputMessage) {
    Message msg = MessageTranslator::deserialize("ResponseTextInput:Alice|name_prompt");

    EXPECT_EQ(msg.type, MessageType::ResponseTextInput);

    const auto* data = std::get_if<ResponseTextInputMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->input, "Alice");
    EXPECT_EQ(data->promptReference, "name_prompt");
}

TEST(MessageTranslatorTest, DeserializesResponseChoiceInputMessage) {
    Message msg = MessageTranslator::deserialize("ResponseChoiceInput:Rock|p1_choice");

    EXPECT_EQ(msg.type, MessageType::ResponseChoiceInput);

    const auto* data = std::get_if<ResponseChoiceInputMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->choice, "Rock");
    EXPECT_EQ(data->promptRef, "p1_choice");
}

TEST(MessageTranslatorTest, DeserializesResponseRangeInputMessage) {
    Message msg = MessageTranslator::deserialize("ResponseRangeInput:5|range_prompt");

    EXPECT_EQ(msg.type, MessageType::ResponseRangeInput);

    const auto* data = std::get_if<ResponseRangeInputMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
    EXPECT_EQ(data->value, 5);
    EXPECT_EQ(data->promptRef, "range_prompt");
}

TEST(MessageTranslatorTest, UnknownPrefixReturnsEmptyMessage) {
    Message msg = MessageTranslator::deserialize("DeleteServer:now");

    EXPECT_EQ(msg.type, MessageType::Empty);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(msg.data));
}

TEST(MessageTranslatorTest, SerializesJoinLobbyMessage) {
    Message msg{
        MessageType::JoinLobby,
        JoinLobbyMessage{"Alice", "Lobby1", 0}
    };

    EXPECT_EQ(
        MessageTranslator::serialize(msg),
        "InternalJoinLobbyAlice|Lobby1|0"
    );
}

TEST(MessageTranslatorTest, SerializesResponseChoiceInputMessage) {
    Message msg{
        MessageType::ResponseChoiceInput,
        ResponseChoiceInputMessage{"Rock", "p1_choice"}
    };

    EXPECT_EQ(
        MessageTranslator::serialize(msg),
        "ResponseChoiceInput:Rock|p1_choice"
    );
}

TEST(MessageTranslatorTest, DeserializesCreateLobbyMessage) 
{ 
    Message msg = MessageTranslator::deserialize("CreateLobby"); 
    EXPECT_EQ(msg.type, MessageType::CreateLobby); 
    EXPECT_TRUE(std::holds_alternative<CreateLobbyMessage>(msg.data)); 
}

TEST(MessageTranslatorTest, DeserializesStartJoinLobbyMessage) 
{ 
    Message msg = MessageTranslator::deserialize("JoinLobby"); 
    EXPECT_EQ(msg.type, MessageType::StartJoinLobby); 
    EXPECT_TRUE(std::holds_alternative<StartJoinLobbyMessage>(msg.data)); 
}

TEST(MessageTranslatorTest, RejectsTrailingDataForCommandOnlyMessages) {
    const std::vector<std::string_view> malformedPayloads{
        "CreateLobbyExtra",
        "JoinLobbyExtra",
        "GetLobbyStateExtra",
    };

    for (const auto payload : malformedPayloads) {
        SCOPED_TRACE(payload);
        expectMalformed(payload);
    }
}

TEST(MessageTranslatorTest, RejectsMissingRequiredFields) {
    const std::vector<std::string_view> malformedPayloads{
        "StartGame:",
        "InternalJoinLobbyAlice",
        "InternalJoinLobbyAlice|Lobby1",
        "InternalJoinLobby|Lobby1|1",
        "InternalJoinLobbyAlice||1",
        "LeaveLobby",
        "BrowseLobbies",
        "ResponseTextInput:Alice",
        "ResponseTextInput:Alice|",
        "ResponseChoiceInput:Rock",
        "ResponseChoiceInput:|p1_choice",
        "ResponseChoiceInput:Rock|",
        "ResponseRangeInput:5",
        "ResponseRangeInput:5|",
    };

    for (const auto payload : malformedPayloads) {
        SCOPED_TRACE(payload);
        expectMalformed(payload);
    }
}

TEST(MessageTranslatorTest, RejectsInvalidNumericFields) {
    const std::vector<std::string_view> malformedPayloads{
        "UpdateCycle",
        "UpdateCycleabc",
        "UpdateCycle1abc",
        "InternalJoinLobbyAlice|Lobby1|abc",
        "InternalJoinLobbyAlice|Lobby1|1abc",
        "InternalJoinLobbyAlice|Lobby1|-1",
        "InternalJoinLobbyAlice|Lobby1|3",
        "BrowseLobbiesabc",
        "BrowseLobbies1abc",
        "BrowseLobbies-1",
        "BrowseLobbies3",
        "ResponseRangeInput:abc|range_prompt",
        "ResponseRangeInput:5abc|range_prompt",
    };

    for (const auto payload : malformedPayloads) {
        SCOPED_TRACE(payload);
        expectMalformed(payload);
    }
}

TEST(MessageTranslatorTest, RejectsUnexpectedExtraFields) {
    const std::vector<std::string_view> malformedPayloads{
        "InternalJoinLobbyAlice|Lobby1|1|extra",
        "ResponseTextInput:Alice|name_prompt|extra",
        "ResponseChoiceInput:Rock|p1_choice|extra",
        "ResponseRangeInput:5|range_prompt|extra",
    };

    for (const auto payload : malformedPayloads) {
        SCOPED_TRACE(payload);
        expectMalformed(payload);
    }
}

TEST(MessageTranslatorTest, AllowsEmptyTextInputForGameLevelValidation) {
    Message msg = MessageTranslator::deserialize(
        "ResponseTextInput:|name_prompt"
    );

    EXPECT_EQ(msg.type, MessageType::ResponseTextInput);

    const auto* data = std::get_if<ResponseTextInputMessage>(&msg.data);
    ASSERT_NE(data, nullptr);
    EXPECT_TRUE(data->input.empty());
    EXPECT_EQ(data->promptReference, "name_prompt");
}
