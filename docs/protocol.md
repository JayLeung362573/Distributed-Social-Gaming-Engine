# WebSocket Message Protocol

This project uses a prefix-based text protocol over WebSocket. Incoming network payloads are parsed by `MessageTranslator` into typed C++ message variants, then routed through `NetworkManager` and `GameServer`.

The protocol is intentionally simple for browser-client testing. Each message starts with a known string prefix, followed by optional payload fields.

## Format

```text
Prefix[:payload]
```

Some payloads use `|` as a field delimiter.

Examples:

```text
StartGame:Alice
InternalJoinLobbyAlice|Lobby1|0
LeaveLobbyAlice
BrowseLobbies0
ResponseChoiceInput:Rock|p1_choice
ResponseTextInput:Alice|name_prompt
ResponseRangeInput:5|range_prompt
```

## Client Messages

| Action | Wire Format | Meaning |
|---|---|---|
| Create lobby flow | `CreateLobby` | Start the interactive lobby creation flow. |
| Start join lobby flow | `JoinLobby` | Start the interactive lobby join flow. |
| Start game | `StartGame:<playerName>` | Request starting the game in the current lobby. |
| Direct join lobby | `InternalJoinLobby<playerName>\|<lobbyName>\|<gameType>` | Request joining a specific lobby using explicit fields. |
| Leave lobby | `LeaveLobby<playerName>` | Leave the current lobby. |
| Browse lobbies | `BrowseLobbies<gameType>` | Request available lobbies for a game type. |
| Get lobby state | `GetLobbyState` | Request the current lobby state. |
| Send text input | `ResponseTextInput:<input>\|<promptRef>` | Send a text input response. Empty input is allowed, but `promptRef` is required. |
| Send choice input | `ResponseChoiceInput:<choice>\|<promptRef>` | Send a choice input response. |
| Send range input | `ResponseRangeInput:<value>\|<promptRef>` | Send a numeric range input response. |

## Server Messages

| Message | Wire Format | Meaning |
|---|---|---|
| Lobby state | `LobbyState:<currentLobbyID>` | Send lobby state after join, leave, or state requests. |
| Text prompt | `RequestTextInput:<prompt>` | Request text input from a client. |
| Choice prompt | `RequestChoiceInput:<prompt>` | Request choice input from a client. |
| Range prompt | `RequestRangeInput:<prompt>\|<min>\|<max>` | Request range input from a client with numeric limits. |
| Game output | `GameOutput:<message>` | Send game output text to a client. |
| Game over | `GameOver:<winner>` | Announce the game result. |
| Error | `Error:<reason>` | Report invalid requests or server-side validation failures. |

## Defensive Handling

The WebSocket layer applies defensive checks before messages reach game logic.

Current safeguards include:

- Payload-size limit: payloads larger than `4096` bytes are dropped.
- Incoming buffer limit: the incoming message buffer is capped at `2048` messages.
- Prefix validation: unknown message prefixes are treated as malformed messages.
- Required-field validation: messages with missing required fields, empty required fields, invalid numeric fields, or unexpected extra delimiters are rejected.
- Malformed-message filtering: invalid messages are not passed to `GameServer`.
- Connection cleanup: disconnected clients are removed from the active connection map.

## Validation Notes

Unknown prefixes are rejected by returning an empty message before reaching normal game-message handling.

The translator also rejects malformed payloads that match a known prefix but fail required-field validation. Rejected cases include:

- Command-only messages with trailing payload data, such as `CreateLobbyExtra`
- Missing required fields, such as `InternalJoinLobbyAlice`
- Empty required fields, such as `InternalJoinLobbyAlice||1`
- Invalid numeric fields, such as `BrowseLobbiesabc`
- Partially parsed numeric fields, such as `BrowseLobbies1abc`
- Unexpected extra fields, such as `ResponseChoiceInput:Rock|p1_choice|extra`

Valid game type values are:

| Value | Meaning |
|---:|---|
| `0` | Default |
| `1` | Number Battle |
| `2` | Choice Battle |

`ResponseTextInput:<input>|<promptRef>` allows an empty text input so game-level validation can decide whether that input is acceptable. The `promptRef` field is still required.

## Malformed Message Examples

### Unknown Prefix

```text
DeleteServer:now
```

Expected behavior:

- The translator returns an empty message because no registered prefix matches.
- No lobby or game state should be modified by normal game-message handling.
- The server continues running.

### Missing Required Fields

```text
InternalJoinLobbyAlice
```

Expected behavior:

- The translator returns an empty message because required lobby and game-type fields are missing.
- The WebSocket layer drops the malformed payload.
- No lobby or game state should be modified.

### Invalid Numeric Field

```text
BrowseLobbies1abc
```

Expected behavior:

- The translator returns an empty message because numeric parsing must consume the full field.
- The WebSocket layer drops the malformed payload.
- No lobby or game state should be modified.

### Unexpected Extra Field

```text
ResponseChoiceInput:Rock|p1_choice|extra
```

Expected behavior:

- The translator returns an empty message because the message contains more fields than the protocol allows.
- The WebSocket layer drops the malformed payload.
- No lobby or game state should be modified.

### Oversized Payload

```text
ResponseTextInput:<payload larger than 4096 bytes>
```

Expected behavior:

- The payload is dropped before reaching game logic.
- The server avoids excessive memory use from untrusted input.

## Implementation Notes

The protocol is implemented mainly in:

```text
src/Network/MessageTranslator.cpp
```

This protocol is not JSON. It is a compact text protocol designed for simple browser-client interaction and typed dispatch in C++.

## Limitations

This protocol is designed for a single-server multiplayer session architecture. It does not currently provide cross-server replication, distributed consensus, persistent recovery, or horizontal scaling across multiple server nodes.