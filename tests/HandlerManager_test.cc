#include <string>
#include "gtest/gtest.h"
#include "HandlerManager.h"
#include "reply.h"

using namespace std;

class HandlerManager_test : public testing::Test
{
  protected:
    Request request;
    NginxConfig config;
    HandlerManager handler_manager;

    virtual void SetUp()
    {
        Request::Request::HeaderItem header_item;
        header_item.name = "Host";
        header_item.value = "localhost:2019";
        request.headers.push_back(header_item);
        request.versionMajor = 1;
        request.versionMinor = 1;
    }

    virtual void TearDown() {}
};


TEST_F(HandlerManager_test, echoHandler)
{
    request.method = "GET";
    request.uri = "/echo";

    string res1 = handler_manager.createByName("ECHO", config, "/")->HandleRequest(request)->give_reply();

    echoHandler echo_handler(config, "/");
    string res2 = echo_handler.HandleRequest(request)->give_reply();
    EXPECT_EQ(res1, res2);
}

TEST_F(HandlerManager_test, staticHandler)
{
    request.method = "GET";
    request.uri = "/static";
    request.file_path = "../tests/staticTest/1.txt";

    string res1 = handler_manager.createByName("STATIC", config, "/")->HandleRequest(request)->give_reply();

    staticHandler static_handler(config, "/");
    string res2 = static_handler.HandleRequest(request)->give_reply();
    EXPECT_EQ(res1, res2);
}

TEST_F(HandlerManager_test, statusHandler)
{
    request.method = "GET";
    request.uri = "/status";

    string res1 = handler_manager.createByName("STATUS", config, "/")->HandleRequest(request)->give_reply();

    statusHandler status_handler(config, "/");
    string res2 = status_handler.HandleRequest(request)->give_reply();

    EXPECT_EQ(res1, res2);
}

TEST_F(HandlerManager_test, healthHandler)
{
    request.method = "GET";
    request.uri = "/health";

    string res1 = handler_manager.createByName("HEALTH", config, "/")->HandleRequest(request)->give_reply();
    
    healthHandler health_handler(config, "/");
    string res2 = health_handler.HandleRequest(request)->give_reply();

    EXPECT_EQ(res1, res2);
}

TEST_F(HandlerManager_test, malformHandler)
{
    request.method = "GET";
    request.uri = "/mime/view/?1d=wow";

    string res1 = handler_manager.createByName("MALFORM", config, "/")->HandleRequest(request)->give_reply();

    malformHandler malform_handler(config, "/");
    string res2 = malform_handler.HandleRequest(request)->give_reply();

    EXPECT_EQ(res1, res2);
}