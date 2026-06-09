# C++ WebSocket Multiplayer Game Server

A C++23 WebSocket multiplayer game server with typed protocol handling, lobby/session management, and server-authoritative game state coordination.

This project demonstrates a backend-oriented networking architecture for real-time multiplayer browser clients. The server accepts WebSocket connections, parses structured client messages, routes them through a network manager, manages lobby/session lifecycle state, and applies defensive checks such as payload-size limits and malformed-message filtering.

The current implementation focuses on a single-server architecture. It is designed as a foundation for multiplayer session management rather than a full distributed system with multi-node state replication or horizontal scaling.

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

The server is authoritative over lobby membership and game session state. Clients send intent-based messages, while the server validates requests, updates server-side state, and broadcasts responses to connected clients.

See [docs/protocol.md](docs/protocol.md) for the WebSocket message format and supported protocol prefixes.

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

## Running with Docker

This project uses C++23, Boost.Asio, and WebSocket networking. Docker is recommended for a reproducible local build environment.

### Build Docker image

1. From the project root:

```bash
docker build -t social-game-dev .
```
2. Start development container

```
docker run -it --rm -p 8080:8080 -v "$PWD":/app social-game-dev
```

3. Build inside Docker
```
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -G Ninja
cmake --build build
```

4. Run server
```
cd build
./bin/main
```

5. The server starts on port 8080.

Open the browser client at:
```
http://localhost:8080
```