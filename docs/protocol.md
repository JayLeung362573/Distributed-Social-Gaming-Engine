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
| Start game | `StartGame:<playerName>` | Request starting the game in the current lobby. |
| Join lobby | `InternalJoinLobby<playerName>\|<lobbyName>\|<gameType>` | Request joining a specific lobby. |
| Leave lobby | `LeaveLobby<playerName>` | Leave the current lobby. |
| Browse lobbies | `BrowseLobbies<gameType>` | Request available lobbies. |
| Get lobby state | `GetLobbyState` | Request the current lobby state. |
| Send text input | `ResponseTextInput:<input>\|<promptRef>` | Send a text input response. |
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
- Malformed-message filtering: invalid messages are not passed to `GameServer`.
- Connection cleanup: disconnected clients are removed from the active connection map.

## Validation Notes

Unknown prefixes are rejected by returning an empty message before reaching normal game-message handling.

Some malformed payloads are still parsed with default or empty fields by the current translator. For example, `InternalJoinLobbyAlice` can still be parsed as a join-lobby message with missing lobby/game fields. Stricter required-field validation is planned for automated protocol tests.

## Malformed Message Examples

### Unknown Prefix

```text
DeleteServer:now
```

Expected behavior:

- The translator returns an empty message because no registered prefix matches.
- No lobby or game state should be modified by normal game-message handling.
- The server continues running.

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