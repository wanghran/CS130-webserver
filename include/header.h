#ifndef HEADER_H
#define HEADER_H

#include <string>

/**
 * An implementation of header class whose job is constructing 
 * a proper header for the HTTP response. The supported functionalities
 * for now are 200 and 404 status, content-type, content-length, 
 * connection close, connection keep-alive, and cotent-transfer-encoding binary.
*/
class header
{
  private:
    std::string OK_status = "HTTP/1.1 200 OK\r\n";
    std::string not_found_status = "HTTP/1.1 404 Not Found\r\n";
    std::string Bad_request = "HTTP/1.1 400 Bad Request\r\n";
    std::string Service_unavailable = "HTTP/1.1 503 Service Unavailable\r\n";
    std::string content_type = "Content-Type: ";
    std::string content_length = "Content-Length: ";
    std::string connection_close = "Connection: close\r\n";
    std::string binary_encoding = "Content-Transfer-Encoding: binary\r\n";
    std::string connection_keep_alive = "Connection: Keep-Alive\r\n";
    std::string keep_alive = "Keep-Alive: ";
    std::string head_file = "";

    /**
     * Current supported status codes which are
     * 200 OK and 404 Not Found.
    */
    enum status
    {
      OK = 200,
      NOT_FOUND = 404,
      BAD_REQUEST = 400,
      SERVICE_UNAVAILABLE = 503
    };

  public: 
    header();

    /**
     * Set the status line for the response header.
     *
     * @param the standard HTTP 1.1 status code.
    */
    void add_status(int status_code);

    /**
     * Set the MIME type of the message body.
     *  
     * @param the correct form of mime type
     *   for example: "text/plain" or "image/jpeg".
    */
    void add_content_type(std::string type);

    /**
     * Set the content-length.
     * 
     * @param the correct length of the message body.
    */ 
    void add_content_length(int length);

    /**
     * Set connection close header line.
    */ 
    void add_connection_close();

    /**
     * Set connection keep-alive and specify the timout and max.
     * 
     * @param the timeout and max value for this connection.
    */ 
    void add_connect_alive(int timeout, int max);

    /**
     * Set the content-transfer-encoding: binary header line.
    */ 
    void add_binary_encoding();

    /**
     * Return the whole header to the caller in the form of string.
     * 
     * @return the whole header with correct form in string.
    */ 
    std::string give_header();
};
#endif