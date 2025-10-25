#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "WebSocketNetworking.h"
#include "GameServer.h"

using ::testing::_;
using ::testing::Exactly;
using ::testing::Ref;

static constexpr unsigned short TestPort = 8080;
static const std::string DummyHtml = "tests/dummy_ws.html";

class MockGameServer : public GameServer {
public:
    MockGameServer() : GameServer(nullptr) {}
    MOCK_METHOD(void, onMessageFromClient, (int, Message&), (override));
};

TEST(WebSocketNetworkingTest, ConstructOK) {
    WebSocketNetworking ws(TestPort, DummyHtml);
    SUCCEED();
}

TEST(WebSocketNetworkingTest, InitiallyNoClients) {
    WebSocketNetworking ws(TestPort, DummyHtml);
    auto ids = ws.getConnectedClientIDs();
    EXPECT_TRUE(ids.empty());
}

TEST(WebSocketNetworkingTest, ReturnEmptyWhenNoActivity) {
    WebSocketNetworking ws(TestPort, DummyHtml);
    auto messages = ws.update();
    EXPECT_TRUE(messages.empty());
}

TEST(WebSocketNetworkingTest, StartServerThenUpdate) {
    WebSocketNetworking ws(TestPort, DummyHtml);
    ws.startServer();
    auto messages = ws.update();
    EXPECT_TRUE(messages.empty());
}

TEST(WebSocketNetworkingTest, ConstructWithMissingHtmlIsSafe) {
    WebSocketNetworking ws(8081, "tests/NoFile.html");
    SUCCEED();
}


TEST(WebSocketNetworking_GMock, SendMessageToServer_CallsGameServerOnce) {
    WebSocketNetworking ws(TestPort, DummyHtml);

    auto mock = std::make_shared<MockGameServer>();
    ws.setServer(mock);

    Message msg{};
    EXPECT_CALL(*mock, onMessageFromClient(1234, Ref(msg)))
        .Times(Exactly(1));

    ws.sendMessageToServer(7, msg);
}

TEST(WebSocketNetworking_GMock, SendMessageToServer_NoServer_NoCrash) {
    WebSocketNetworking ws(TestPort, DummyHtml);
    Message msg{};
    EXPECT_NO_THROW(ws.sendMessageToServer(42, msg));
}