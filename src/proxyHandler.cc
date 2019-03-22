#include <stdio.h>
#include <boost/asio.hpp>
#include <string>
#include "proxyHandler.h"

using namespace std;
using boost::asio::ip::tcp;

proxyHandler::proxyHandler(const NginxConfig& config, const std::string& root_path) 
{
	this->config = config;
	this->root_path = root_path;
}

proxyHandler* proxyHandler::create(const NginxConfig& config, const string& root_path)
{
	proxyHandler* proxy_handler = new proxyHandler(config, root_path);
	return proxy_handler;
}

std::unique_ptr<reply> proxyHandler::HandleRequest(const Request& request)
{
	string content;

	// TODO: Handle other codes (e.g. redirect, 404)

	// Send Request & Receive Response from remote server.
	// Credits: https://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio/example/http/client/sync_client.cpp
	boost::asio::io_service io_service;
	tcp::resolver resolver(io_service);
	tcp::resolver::query query(request.file_path, request.proxy_port);
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	tcp::socket socket(io_service);
	boost::asio::connect(socket, endpoint_iterator);

	boost::asio::streambuf proxyRequest;
	std::ostream request_stream(&proxyRequest);
	request_stream << "GET " << request.file_name << " HTTP/1.1\r\n";
	request_stream << "Host: " << request.file_path << "\r\n";
	request_stream << "Connection: keep-alive\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	boost::asio::write(socket, proxyRequest);

	boost::asio::streambuf proxyResponse;
	boost::asio::read_until(socket, proxyResponse, "\r\n");

	std::istream response_stream(&proxyResponse);
	std::string http_version;
	response_stream >> http_version;
	unsigned int status_code;
	response_stream >> status_code;
	std::string status_message;
	std::getline(response_stream, status_message);

	std::string content_type;

	if ((status_code != 200 && status_code != 301 && status_code != 302 && status_code != 303) || !response_stream || http_version.substr(0, 5) != "HTTP/")
	{
		content = "Status code is " + to_string(status_code);
	}
	else
	{
		boost::asio::read_until(socket, proxyResponse, "\r\n\r\n");

		std::ostringstream ss;
		std::string proxyHeader;

		while (std::getline(response_stream, proxyHeader) && proxyHeader != "\r") {
			if (proxyHeader.find("Content-Type") != string::npos)
				content_type = proxyHeader.substr(13, proxyHeader.find(";") - 13) + '\0';
		}
		//ss << "\n";

		// Get data
		if (proxyResponse.size() > 0)
			ss << &proxyResponse;

		boost::system::error_code error;
		while (boost::asio::read(socket, proxyResponse, boost::asio::transfer_at_least(1), error))
			ss << &proxyResponse;
		content = ss.str();
	}

	if (status_code == 301 || status_code == 302 || status_code == 303){
		unique_ptr<reply> response(new reply);

		response->set_header(status_code);
		response->set_mime(content_type);
		response->set_keep_alive(5, 1000);
		response->set_length(content.size());
		response->set_content(content);
		return response;
	}

	if (content_type.find("text/html")) {
		// parse through the HTML and for every source, attach the hostname.
		size_t srcPosition = content.find("src=\"/");
		while (srcPosition != string::npos) {
			content.insert(srcPosition + 5, "http://" + request.file_path);
			srcPosition = content.find("src=\"/", srcPosition + 1);
		}
		size_t hrefPosition = content.find("href=\"/");
		while (hrefPosition != string::npos) {
			content.insert(hrefPosition + 6, "http://" + request.file_path);
			hrefPosition = content.find("href=\"/", hrefPosition + 1);
		}
	}

	unique_ptr<reply> response(new reply);

	response->set_header(status_code);
	response->set_mime(content_type);
	response->set_keep_alive(5, 1000);
	response->set_length(content.size());
	response->set_content(content);

	return response;
}
