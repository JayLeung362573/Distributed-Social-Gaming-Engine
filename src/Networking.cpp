#include "Networking.h"
#include "GameClient.h"
#include "GameServer.h"


void InMemoryNetworking::setServer(std::shared_ptr<GameServer> server)
{
    m_server = server;
}

void InMemoryNetworking::addClient(std::shared_ptr<GameClient> client)
{
    m_clients[client->getClientID()] = client;
}

void InMemoryNetworking::sendMessageToClient(int toClientID, Message &message)
{
    if (m_clients.contains(toClientID))
    {
        m_clients[toClientID]->onMessageFromServer(message);
    }
}

void InMemoryNetworking::sendMessageToServer(int fromClientID, Message &message)
{
    if (m_server)
    {
        m_server->onMessageFromClient(fromClientID, message);
    }
}
