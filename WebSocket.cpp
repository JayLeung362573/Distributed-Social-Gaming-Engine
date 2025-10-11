#include "WebSocket.h"
#include "GameServer.h"

#include <string>
#include <deque>
#include <algorithm>
#include <fstream>

static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    return {std::istreambuf_iterator<char>(file), { }};
}

WebSocket::WebSocket(unsigned short port, const std::string& htmlPath) {
    auto html = readFile(htmlPath);
    wsServer = std::make_unique<networking::Server>(
        port, html,
        [this](networking::Connection c){ handleConnect(c);},
        [this] (networking::Connection c) {handleDisconnect(c); }
    );
}

void WebSocket::setServer(std::shared_ptr<GameServer> s) {
    gameServer = std::move(s);
}

void WebSocket::handleConnect(networking::Connection c) {
    connections.push_back(c);
    std::string name = "Player" + std::to_string(c.id);
    players[c.id] = name;

    if(gameServer) {
        Message joinMsg {MessageType::JoinGame, JoinGameMessage{name} };
        gameServer->onMessageFromClient(c.id, joinMsg);
    }
}

void WebSocket::handleDisconnect(networking::Connection c) {
    players.erase(c.id);
}


void WebSocket::update() {
    wsServer->update();
}

void WebSocket::sendMessageToClient(int, Message&) {}
void WebSocket::sendMessageToServer(int, Message&) {}
