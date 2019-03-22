#include "gtest/gtest.h"
#include "staticHandler.h"
#include "header.h"
#include "reply.h"
#include "config_parser.h"
#include <vector>
#include <iostream>

using namespace std;

TEST(staticHandler_get_path, filePath)
{
    Request request;
	NginxConfig config;
    request.file_path = "../tests/staticTest/1.txt";
    staticHandler static_handler(config,"/");
    static_handler.HandleRequest(request);
    EXPECT_EQ("../tests/staticTest/1.txt", static_handler.get_file_path());
}

TEST(staticHandler_type_test, txtFile)
{
    Request request;
	NginxConfig config;
    request.file_path = "../tests/staticTest/1.txt";
    staticHandler static_handler(config,"/");
    static_handler.HandleRequest(request);
    EXPECT_EQ(staticHandler::fileType::txt, static_handler.getFileType());
}

TEST(staticHandler_type_test, jpgFile)
{
    Request request;
	NginxConfig config;
    request.file_path = "../tests/staticTest/1.jpg";
    staticHandler static_handler(config,"/");
    static_handler.HandleRequest(request);
    EXPECT_EQ(staticHandler::fileType::jpg, static_handler.getFileType());
}

TEST(staticHandler_type_test, htmlFile)
{
    Request request;
	NginxConfig config;
    request.file_path = "../tests/staticTest/1.htm";
    staticHandler static_handler(config,"/");
    static_handler.HandleRequest(request);
    EXPECT_EQ(staticHandler::fileType::html, static_handler.getFileType());
}

TEST(staticHandler_type_test, zipFile)
{
    Request request;
	NginxConfig config;
    request.file_path = "../tests/staticTest/1.zip";
    staticHandler static_handler(config,"/");
    static_handler.HandleRequest(request);
    EXPECT_EQ(staticHandler::fileType::zip, static_handler.getFileType());
}

TEST(staticHandler_unsupported_fileType, xmlFile)
{
    Request request;
	NginxConfig config;
    request.file_path = "../tests/staticTest/1.xml";
    staticHandler static_handler(config,"/");
    string response = static_handler.HandleRequest(request)->give_reply();
    string shouldBe = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nContent-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n";
    EXPECT_EQ(response, shouldBe);
}


TEST(staticHandler_txtResponse, txtFile)
{
    Request request;
	NginxConfig config;
    request.file_path = "../tests/staticTest/1.txt";
    staticHandler static_handler(config,"/");
    string response = static_handler.HandleRequest(request)->give_reply();
    string shouldBe("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 25\r\nContent-Transfer-Encoding: binary\r\n"
                    "Connection: Keep-Alive\r\nKeep-Alive: timeout=5, max=1000\r\n\r\n"
                    "hahahahahahahahahahahahah");
    EXPECT_EQ(response, shouldBe);
}
