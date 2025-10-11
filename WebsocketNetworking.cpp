#include "WebSocketNetworking.h"
#include "GameServer.h"
#include "GameClient.h"

WebSocketNetworking::WebSocketNetworking(unsigned short port)
{
    std::string httpPage = "<html><body><h1>WebSocket Server Ready</h1></body></html>";

    // Create server with callbacks
    net_server = std::make_unique<networking::Server>(
        port,
        httpPage,
        [this](networking::Connection c) {
            int fromClientID = c.id;
            std::cout << "[WebSocket] Client connected (id=" << fromClientID << ")\n";

            if (!m_clients.contains(fromClientID) && m_server && has_server_started) {
                auto client = std::make_shared<GameClient>(fromClientID, shared_from_this());
                m_clients[fromClientID] = client;
            }

        },
        [this](networking::Connection c) {
            int fromClientID = c.id;
            std::cout << "[WebSocket] Client disconnected (id=" << fromClientID << ")\n";

            m_clients.erase(fromClientID);
        }
    );

    std::cout << "[WebSocket] Server initialized on port " << port << "\n";
}

void WebSocketNetworking::setServer(std::shared_ptr<GameServer> server) {
    m_server = server;
}

void WebSocketNetworking::startServer()
{
    std::cout << "[WebSocket] Server started.\n";
    has_server_started = true;
}

void WebSocketNetworking::update()
{
    try {
        net_server->update();

        auto messages = net_server->receive();

        for (auto& msg : messages) {
            std::cout << "[WebSocket] Received: " << msg.text << " from client " << msg.connection.id << "\n";

            // convert incoming networking::Message to our custom Message object
            Message gameMsg = deserialize(msg.text);

            int fromClientID  = msg.connection.id ? msg.connection.id : 0;

            // Pass it up to the game logic
            sendMessageToServer(fromClientID, gameMsg);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << "\n";
    }
}

void WebSocketNetworking::sendMessageToClient(int toClientID, Message& message)
{
    std::string payload = serialize(message);

    // Convert our message into network::Message compatible with the web-socket format to send over the network
    std::deque<networking::Message> out{
        networking::Message{networking::Connection{(uintptr_t)toClientID}, payload}
    };

    net_server->send(out);
}

void WebSocketNetworking::sendMessageToServer(int fromClientID, Message& message)
{
    // Called by GameClient
    if (m_server) {
        m_server->onMessageFromClient(fromClientID, message);
    }
}

// tokenize payload to client
std::string WebSocketNetworking::serialize(const Message& msg)
{
    if (msg.type == MessageType::JoinGame) {
        auto& data = std::get<JoinGameMessage>(msg.data);
        return "JoinGame:" + data.playerName;
    }
    return "Empty";
}

// parse the tokens from client
Message WebSocketNetworking::deserialize(const std::string& payload)
{
    if (payload.starts_with("JoinGame:")) {
        std::string name = payload.substr(9);
        return {MessageType::JoinGame, JoinGameMessage{name}};
    }
    return {};
}