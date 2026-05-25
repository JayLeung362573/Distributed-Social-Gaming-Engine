# Distributed Social Gaming Engine

A C++ WebSocket-based multiplayer game server with typed message dispatching, lobby/session management, and server-side game-state coordination.

## Architecture

Client Browser
   ↓ WebSocket
WebSocketNetworking
   ↓ ClientMessage
NetworkManager
   ↓
GameServer
   ↓
LobbyRegistry / GameSession

## Key Features

- WebSocket-based client/server communication
- Event-loop server architecture
- Type-safe message protocol using std::variant
- Table-driven message serialization/deserialization
- Lobby create/join/leave/browse flows
- Server-side session tracking
- Payload size limits and malformed-message filtering