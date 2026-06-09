# Defensive Networking Safeguards

This document summarizes the defensive checks used by the WebSocket networking layer before incoming client messages reach game logic.

The main implementation is located in:

```text
src/Network/WebsocketNetworking.cpp
```

## Safeguards

### 1. Payload-Size Limit

- Threshold: `4096` bytes
- Behavior: Incoming WebSocket payloads larger than `4096` bytes are automatically dropped before translation.
- Purpose: Prevents oversized, untrusted input from reaching the protocol parser (`MessageTranslator`) or the core game server.

### 2. Incoming-Buffer Cap

- Threshold: `2048` messages
- Behavior: The server caps the incoming message buffer at `2048` messages. When the buffer is full, additional incoming messages are dropped.
- Purpose: Prevents unbounded memory growth and protects against simple flooding attempts.

### 3. Malformed-Message Filtering

- Behavior: Incoming payloads are translated through `MessageTranslator`. If the translator returns an empty message for a non-empty payload, the WebSocket layer treats it as malformed and blocks it from entering the game-server queue.
- Purpose: Ensures only syntactically valid and recognized messages reach the active game logic.

### 4. Disconnected-Client Cleanup

- Behavior: When a client disconnects, its connection ID is immediately removed from the active connection map.
- Purpose: Cleans up internal state and prevents the server from attempting to route messages to stale or dead WebSocket connections.

## Current Limitations

While these safeguards protect the single-server WebSocket layer from basic malformed input and unbounded buffering, the current architecture does not implement:

- Client authentication / authorization
- Per-client granular rate limiting
- Distributed session recovery
- Persistent reconnection state management