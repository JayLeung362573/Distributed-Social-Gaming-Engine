#include "Message.h"
#include "Networking.h"
#include "GameClient.h"
#include "GameServer.h"

// simple demo of two clients sending a JoinGame message to the server
int main()
{
    int CLIENT_1_ID = 100;
    int CLIENT_2_ID = 101;

    auto networking = std::make_shared<InMemoryNetworking>();

    auto server = std::make_shared<GameServer>(networking);
    auto client1 = std::make_shared<GameClient>(CLIENT_1_ID, networking);
    auto client2 = std::make_shared<GameClient>(CLIENT_2_ID, networking);

    networking->setServer(server);
    networking->addClient(client1);
    networking->addClient(client2);

    Message msg1{MessageType::JoinGame, JoinGameMessage{"joe"}};
    Message msg2{MessageType::JoinGame, JoinGameMessage{"amy"}};

    client1->sendMessageToServer(msg1);
    client2->sendMessageToServer(msg2);

    // Expected output:
    // [server] Got message from client (ID=100) : JoinGame("joe")
    // [server] Got message from client (ID=101) : JoinGame("amy")
}
