#include <string>
#include "gtest/gtest.h"
#include "mimeHandler.h"
#include "header.h"
#include "reply.h"
#include "config_parser.h"

class mimeHandler_test : public testing::Test
{
	protected:
	Request request;
	NginxConfig config;

	virtual void SetUp()
	{
		Request::Request::HeaderItem header_item;
		header_item.name = "Host";
		header_item.value = "localhost:2019";
		request.headers.push_back(header_item);
		request.versionMajor = 1;
		request.versionMinor = 1;
		request.method = "GET";
		request.uri = "/mime/view";
	}

	virtual void TearDown() {}
};

TEST_F(mimeHandler_test, view_200_response)
{
    request.file_name = "=1";
    request.request_uri = "/mime/view?id=1";
    request.query = "id=1";

	mimeHandler mime_handler(config, "/");
    std::string ret = mime_handler.view(request, 1)->give_reply();
	std::string shouldBe("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 378\r\n"
                            "Content-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n"
                            "<!DOCTYPE html>\n<html>\n<style>\nbody { display: inline-block; position: relative; }\n"
                            "span { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }\n"
                            "#top { top: 0; }\n#bottom { bottom: 0; }\n</style>\n<body>\r\n"
                            "<img src=\"data:image/jpeg;base64,Donald Trump\">\r\n<span id=\"top\">Donald</span>\r\n"
                            "<span id=\"bottom\">Trump</span>\r\n</body>\r\n</html>");
	EXPECT_EQ(ret, shouldBe);
}

TEST_F(mimeHandler_test, view_400_response)
{
    request.file_name = "=a";
    request.request_uri = "/mime/view?id=a";
    request.query = "id=a";

	mimeHandler mime_handler(config, "/");
    std::string ret = mime_handler.view(request, 1)->give_reply();
	std::string shouldBe("HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nContent-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n");
	EXPECT_EQ(ret, shouldBe);
}

TEST_F(mimeHandler_test, view_404_response)
{
    request.file_name = "=1";
    request.request_uri = "/mime/view?id=1";
    request.query = "id=1";

	mimeHandler mime_handler(config, "/");
    std::string ret = mime_handler.view(request, -1)->give_reply();
	std::string shouldBe("HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nContent-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n");
	EXPECT_EQ(ret, shouldBe);
}

TEST_F(mimeHandler_test, view_503_response)
{
    request.file_name = "=1";
    request.request_uri = "/mime/view?id=1";
    request.query = "id=1";

	mimeHandler mime_handler(config, "/");
    std::string ret = mime_handler.view(request, -2)->give_reply();
	std::string shouldBe("HTTP/1.1 503 Service Unavailable\r\nContent-Length: 0\r\nContent-Transfer-Encoding: binary\r\nConnection: close\r\n\r\n");
	EXPECT_EQ(ret, shouldBe);
}
