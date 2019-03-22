#include "gtest/gtest.h"
#include "UrlProcessor.h"
#include "request.h"

class UrlProcessor_test : public testing::Test
{
	protected:
	Request request;
	NginxConfig config;

	virtual void SetUp ()
	{
		Request::Request::HeaderItem header_item;
		header_item.name = "Host";
		header_item.value = "localhost:2019";
		request.headers.push_back(header_item);
		request.versionMajor = 1;
		request.versionMinor = 1;
		request.method = "GET";
		request.uri = "http://localhost/static";
        config.root_path = "/root";
        config.static_path_.push_back("/static1");
        config.client_access_static_.push_back("/static");
        config.echo_path = "/echo";
        //UrlProcessor pp(&config);
        //p = &pp;
	}

	virtual void TearDown() {
        //delete p;
    }
};

TEST_F(UrlProcessor_test, GetFileName)
{
    UrlProcessor p(&config);
    p.setup(&config);
    std::string path = "/static/1.txt";
    std::string f ="/1.txt";
    std::string filename = p.get_filename(path);
    EXPECT_EQ(filename, f);
}

TEST_F(UrlProcessor_test, CheckDir)
{
    UrlProcessor p(&config);
    p.setup(&config);
    bool re = p.handle_url(request);
    UrlProcessor::RequestContent status = p.check_dir("/static");
    bool result = (status == UrlProcessor::RequestContent::STATIC_);
    EXPECT_TRUE(result);
    status = p.check_dir("/echo");
    result = (status == UrlProcessor::RequestContent::ECHO_);
    EXPECT_TRUE(result);
    status = p.check_dir("/a");
    result = (status == UrlProcessor::RequestContent::INVALID_);
    EXPECT_FALSE(result);
}

TEST_F(UrlProcessor_test, HandleUrl)
{
    UrlProcessor p(&config);
    p.setup(&config);
    bool status = p.handle_url(request);
    EXPECT_FALSE(status);
    request.uri = "http://localhost:2019/echo";
    status = p.handle_url(request);
    EXPECT_TRUE(status);
    request.uri = "http://localhost:2019/sss";
    status = p.handle_url(request);
    EXPECT_TRUE(status);
}

TEST_F(UrlProcessor_test, GetPrivateMember)
{
    UrlProcessor p(&config);
    p.setup(&config);
    request.uri = "http://localhost:2019/static/1.txt";
    bool re = p.handle_url(request);
    std::string file = p.give_filename();
    EXPECT_EQ(file, "/1.txt");
    UrlProcessor::RequestContent result = p.give_RequestContent();
    re = (result == UrlProcessor::STATIC_);
    EXPECT_TRUE(re);
}
