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

## Demo

The server can be run locally with Docker and accessed through the browser client at `http://localhost:8080`.

Example server startup:

```text
[WebSocket] Server initialized on port 8080
[WebSocket] Server started.
```

Example client connection flow:

```text
[WebSocket] Client connected (id=1)
[WebSocket] Received: InternalJoinLobbyAlice|Lobby1|0 from client 1
[Translator] Checking payload: 'InternalJoinLobbyAlice|Lobby1|0'
[WebSocket] Received: ResponseChoiceInput:Rock|p1_choice from client 1
[Translator] Checking payload: 'ResponseChoiceInput:Rock|p1_choice'
```

Example malformed-message handling:

```text
[WebSocket] Received: DeleteServer:now from client 1
[Translator] Checking payload: 'DeleteServer:now'
[Translator] No matching prefix found. Returning Empty.
[WebSocket] Dropped malformed payload from client 1
```

This demonstrates the main runtime path:

```text
Browser client -> WebSocket payload -> MessageTranslator -> typed Message variant -> NetworkManager -> GameServer
```

### Client Simulation

A lightweight Python script can simulate multiple WebSocket clients and send representative protocol messages to the server.

Install the Python dependency:

```bash
python3 -m pip install websockets
```

Run the simulation while the server is running:

```bash
python3 scripts/simulate_clients.py --clients 5
```

Example output:

```bash
Client simulation complete
WebSocket URI: ws://localhost:8080
Clients attempted: 5
Clients connected: 5
Clients failed: 0
Messages sent: 25
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

The project includes automated GoogleTest-based unit tests for core server and protocol behavior.

Current test coverage includes:

- WebSocket networking integration behavior
- Game server behavior
- Lobby registry behavior
- Game session behavior
- Message translation and protocol serialization/deserialization
- Unknown protocol prefix handling

To build and run tests:

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

To run only the protocol tests:

```bash
./build/tests/unit_tests --gtest_filter="MessageTranslatorTest.*"
```

On macOS, building the WebSocket dependency may require a compiler toolchain with newer C++23 ranges support. The Docker environment uses GCC 14 for a more consistent build.

For networking-layer defensive checks such as payload-size limits, incoming-buffer caps, malformed-message filtering, and disconnected-client cleanup, see [docs/networking-safeguards.md](docs/networking-safeguards.md).

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

## Future Work

The current implementation focuses on a single-server WebSocket multiplayer architecture. Future improvements could extend the project toward a more scalable distributed system.

Planned improvements include:

- Add stricter required-field validation for protocol payloads.
- Add automated tests for payload-size limits and incoming-buffer caps.
- Add per-client rate limiting to protect against message spam.
- Add reconnect handling for clients that temporarily disconnect.
- Add persistent lobby/session recovery after server restart.
- Explore horizontal scaling with multiple server instances and shared session state.
- Add matchmaking support for automatically grouping players into compatible lobbies.