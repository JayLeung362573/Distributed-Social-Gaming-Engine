#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <variant>

#include "Types.h"
#include "GameMessage.h"

struct TextInputRequest
{
    String playerID;
    String prompt;
};

struct ChoiceInputRequest
{
    String playerID;
    String prompt;
    List<Value> choices;
};

struct RangeInputRequest
{
    String playerID;
    String prompt;
    int minValue;
    int maxValue;
};

struct VoteInputRequest
{
    String playerID;
    String prompt;
    List<Value> choices;
};

using InputRequest = std::variant<
    TextInputRequest,
    ChoiceInputRequest,
    RangeInputRequest,
    VoteInputRequest
>;

class InputManager {
public:
    InputManager() = default;

    std::optional<String> getTextInput(String playerID, String prompt);
    std::optional<String> getChoiceInput(String playerID, String prompt, const List<Value>& choices);
    std::optional<int> getRangeInput(String playerID, String prompt, int minValue, int maxValue);
    std::optional<String> getVoteInput(String playerID, String prompt, const List<Value>& choices);

    void handleIncomingMessages(const std::vector<GameMessage>& messages);
    std::vector<GameMessage> getPendingRequests();
    void clearPendingRequests();

private:
    std::optional<String> findResponse(const String& playerID, const String& prompt) const;
    bool hasRequestedInput(const String& playerID, const String& prompt) const;
    void addPendingRequest(InputRequest request);

private:
    std::unordered_map<String, std::unordered_map<String, String>> m_responses;
    std::vector<InputRequest> m_pendingRequests;
    std::unordered_map<String, std::unordered_set<String>> m_sentRequests;
};