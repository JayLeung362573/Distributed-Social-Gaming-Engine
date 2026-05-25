# Distributed Social Gaming Engine

A C++ WebSocket-based multiplayer game server with typed message dispatching, lobby/session management, and server-side game-state coordination.

## Architecture

```text
Client Browser
      │
      ▼ WebSocket
WebSocketNetworking
      │
      ▼ ClientMessage
NetworkManager
      │
      ▼
GameServer
      │
      ▼
LobbyRegistry / GameSession
```

## Key Features

- WebSocket-based client/server communication
- Event-loop server architecture
- Type-safe message protocol using std::variant
- Table-driven message serialization/deserialization
- Lobby create/join/leave/browse flows
- Server-side session tracking
- Payload size limits and malformed-message filtering

## Defensive Networking

The WebSocket networking layer includes basic safeguards for server robustness:

- Drops payloads larger than 4096 bytes
- Caps buffered incoming messages at 2048
- Filters malformed messages that cannot be deserialized into known message types
- Removes disconnected clients from the active connection map

## Testing

Manual test scenarios are documented in:

```bash
tests/manual_test_plan.md
```