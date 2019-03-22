#include "request.h"

Request::Request() {}

std::string Request::inspect()
{
    std::stringstream stream;
    stream << method << " " << uri << " HTTP/"
            << versionMajor << "." << versionMinor << "\n";

    for(std::vector<Request::HeaderItem>::const_iterator it = headers.begin();
        it != headers.end(); ++it)
    {
        stream << it->name << ": " << it->value << "\n";
    }

    std::string data(content.begin(), content.end());
    stream << data << "\n";
    stream << "+ keep-alive: " << keepAlive << "\n";;
    return stream.str();
}