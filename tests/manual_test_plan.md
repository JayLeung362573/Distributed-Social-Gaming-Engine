# Manual Test Plan

This document describes manual test scenarios for validating the multiplayer server, WebSocket networking layer, lobby lifecycle, and message handling behavior.

## Environment

- Build system: CMake
- Language: C++23
- Networking: WebSocket-based client/server communication
- Default server port: 8080

## Test 1: Server Startup

### Steps

1. Build the project.
2. Start the game server.
3. Open the browser client page served by the server.
4. Confirm that the server logs show successful startup.

### Expected Result

- Server starts without crashing.
- Browser client can connect to the WebSocket server.
- Server logs show a new client connection.

---

## Test 2: New Client Connection Detection

### Steps

1. Start the server.
2. Open one browser client.
3. Observe the server logs.
4. Open a second browser client.
5. Observe the server logs again.

### Expected Result

- Each browser client receives a unique client ID.
- Server detects each new client.
- Server sends the current lobby list to newly connected clients.

---

## Test 3: Lobby Creation Flow

### Steps

1. Connect one browser client.
2. Send a CreateLobby message.
3. Enter a player name when prompted.
4. Enter a lobby name when prompted.
5. Select a game type.

### Expected Result

- Server asks for player name.
- Server asks for lobby name.
- Server asks for game type.
- Server creates a lobby.
- Client receives a LobbyState message.
- Client receives a success message.

---

## Test 4: Join Existing Lobby

### Steps

1. Start the server.
2. Client A creates a lobby.
3. Client B connects to the server.
4. Client B starts the join lobby flow.
5. Client B enters a player name.
6. Client B enters the lobby name or lobby ID.

### Expected Result

- Client B joins the lobby successfully.
- Client B receives the updated LobbyState.
- Existing lobby members receive a notification that Client B joined.
- Existing lobby members receive an updated LobbyState.

---

## Test 5: Prevent Duplicate Lobby Membership

### Steps

1. Client A creates or joins a lobby.
2. Client A attempts to create another lobby without leaving the current one.

### Expected Result

- Server rejects the request.
- Client receives an Error message explaining that the client is already in a lobby.

---

## Test 6: Leave Lobby

### Steps

1. Client A creates a lobby.
2. Client B joins the lobby.
3. Client B sends a LeaveLobby message.

### Expected Result

- Client B receives leave confirmation.
- Remaining lobby members receive an updated LobbyState.
- If the lobby becomes empty, the server removes it.

---

## Test 7: Start Game Authorization

### Steps

1. Client A creates a lobby as host.
2. Client B joins the lobby.
3. Client B attempts to start the game.
4. Client A attempts to start the game.

### Expected Result

- Server rejects Client B because only the host can start the game.
- Server allows Client A to start the game.
- All lobby players receive StartGame messages.

---

## Test 8: Not Enough Players

### Steps

1. Client A creates a lobby.
2. Client A attempts to start the game before another player joins.

### Expected Result

- Server rejects the request.
- Client A receives an Error message explaining that at least two players are required.

---

## Test 9: Malformed Message Handling

### Steps

1. Connect a browser client.
2. Send a malformed message with an unknown prefix.
3. Observe server logs.

### Expected Result

- Server drops the malformed payload.
- Server does not crash.
- Server continues accepting valid messages afterward.

---

## Test 10: Oversized Payload Handling

### Steps

1. Connect a browser client.
2. Send a payload larger than 4096 bytes.
3. Observe server logs.

### Expected Result

- Server drops the oversized payload.
- Server does not crash.
- Server continues handling later valid messages.

---

## Test 11: Disconnection Cleanup

### Steps

1. Connect one or more browser clients.
2. Close one browser tab.
3. Observe the server logs.
4. Send messages from remaining clients.

### Expected Result

- Server removes the disconnected client from the connection map.
- Remaining clients continue to work normally.