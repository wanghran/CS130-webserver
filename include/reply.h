#ifndef REPLY_H
#define REPLY_H
#include <string>
#include "header.h"

/**
 * An implementation of reply class. 
 * The reply class acts as a wrapper over header class, 
 * which saves all the important information about the response
 * and construct the header when called give_reply. The reply
 * class should not fail.
*/ 
class reply
{
  private:
    header header_line;
    std::string content;
    int error_code = 0;
    int length = 0;
    int stat_code = 0;
    std::string MIME_type;

    /**
     * A data structure to save the information of the timeout and 
     * max, when connection keep-alive is needed.
    */ 
    struct keep_alive
    {
      int timeout = 0;
      int max = 0;
    };
    keep_alive ka;

  public:
    reply();

    /**
     * Initialize the header class with corresponding status code.
     * 
     * @param the correct HTTP 1.1 status code.
    */ 
    void set_header(int status_code);

    /**
     * Save the message body of this response.
     * 
     * @param the message body in string.
    */ 
    void set_content(std::string content);

    /**
     * Set the error code for the reply so not exception is
     * needed to be thrown. (Not implemented yet)
     * 
     * @param error code.
    */ 
    void set_error_code(int error_code);

    /**
     * Save the length of the message body.
     * 
     * @param length of the response message.
    */ 
    void set_length(int length);

    /**
     * Save the mime type of the message body.
     * 
     * @param correct mime type according to HTTP 1.1.
    */ 
    void set_mime(std::string type);

    /**
     * Save the timeout and max to the keep_alive 
     * data structure.
     * 
     * @param value of timeout and max.
    */ 
    void set_keep_alive(int timeout, int max);

    /**
     * Construct the full header by calling methods from header class 
     * and concatenate it with the message body saved in content. Return
     * the full response to the caller.
     * 
     * @return the full HTTP 1.1 response.
    */ 
    std::string give_reply();

    // For proxy server
    std::string give_content();

    std::string give_header();

    /**
     * Get the status code of the response message.
     * 
     * @return the HTTP 1.1 status code.
    */ 
    int get_status_code();
};
#endif
