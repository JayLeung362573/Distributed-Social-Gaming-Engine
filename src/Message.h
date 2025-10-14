#pragma once

#include <memory>
#include <variant>
#include <string>

enum class MessageType : uint8_t
{
    Empty = 0,
    JoinGame,
};

struct JoinGameMessage
{
    std::string playerName;
};

using MessageData = std::variant<std::monostate, JoinGameMessage>;

struct Message
{
    MessageType type = MessageType::Empty;
    MessageData data;
};
