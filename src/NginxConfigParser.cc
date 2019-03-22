// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "config_parser.h"

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    case TOKEN_TYPE_ENDOFLINE:     return "TOKEN_TYPE_ENDOFLINE";
    case TOKEN_TYPE_QUOTED_STRING: return "TOKEN_TYPE_QUOTED_STRING";
    //case TOKEN_TYPE_WHITESPACE:    return "TOKEN_TYPE_WHITESPACE";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  char last;
  int escape = 0;
  char espchar;
  while (input->good()) {
    char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
            //*value = c;
            //return TOKEN_TYPE_WHITESPACE;
          case '\t':
          case '\r':
            continue;
          case '\n':
            *value = c;
            return TOKEN_TYPE_ENDOFLINE;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        // TODO: the end of a quoted token should be followed by whitespace.
        // TODO: Maybe also define a QUOTED_STRING token type.
        // TODO: Allow for backslash-escaping within strings.
        /*if (escape == 1) {
          switch (c) {
            case '\'':
              espchar = '\'';
              c = espchar;
              break;
            case '\"':
              espchar = '\"';
              c = espchar;
              break;
            case '\?':
              espchar = '\?';
              c = espchar;
              break;
            //case '\\':
              //espchar = '\\';
              //c = espchar;
              //break;
            case 'a':
              espchar = '\a';
              c = espchar;
              break;
            case 'b':
              espchar = '\b';
              c = espchar;
              break;
            case 'f':
              espchar = '\f';
              c = espchar;
              break;
            case 'n':
              espchar = '\n';
              c = espchar;
              break;
            case 'r':
              espchar = '\r';
              c = espchar;
              break;
            case 't':
              espchar = '\t';
              c = espchar;
              break;
            case 'v':
              espchar = '\v';
              c = espchar;
              break;
            default:
              espchar = c;
              *value += last;
          }
          //*value += espchar;
          escape = 0;
        }
        if (c != '\\' && escape == 0) {
          *value += c;
        } else {
          escape = 1;
        }*/
        if (c != '\\') {
          *value += c;
        }
        if (c == '\'') {
          if (last == ' ') {
            return TOKEN_TYPE_QUOTED_STRING;
          } else {
            return TOKEN_TYPE_ERROR;
          }
        }
        last = c;
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        /*if (escape == 1) {
          switch (c) {
            case '\'':
              espchar = '\'';
              c = espchar;
              break;
            case '\"':
              espchar = '\"';
              c = espchar;
              break;
            case '\?':
              espchar = '\?';
              c = espchar;
              break;
            //case '\\':
              //espchar = '\\';
              //c = espchar;
              //break;
            case 'a':
              espchar = '\a';
              c = espchar;
              break;
            case 'b':
              espchar = '\b';
              c = espchar;
              break;
            case 'f':
              espchar = '\f';
              c = espchar;
              break;
            case 'n':
              espchar = '\n';
              c = espchar;
              break;
            case 'r':
              espchar = '\r';
              c = espchar;
              break;
            case 't':
              espchar = '\t';
              c = espchar;
              break;
            case 'v':
              espchar = '\v';
              c = espchar;
              break;
            default:
              espchar = c;
              *value += last;
          }
          //*value += espchar;
          escape = 0;
        }
        if (c != '\\' && escape == 0) {
          *value += c;
        } else {
          escape = 1;
        }*/
        if (c != '\\') {
          *value += c;
        }
        if (c == '"') {
          //if (last == ' ') {
            return TOKEN_TYPE_QUOTED_STRING;
          //} else {
            //return TOKEN_TYPE_ERROR;
          //}
        }
        last = c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\t' || c == '\n' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          //if (c == '\n') {
          //input->unget();
            //return TOKEN_TYPE_ENDOFLINE;
          //}
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  bool find_port = false;
  std::string port_target = "listen";
  bool find_root = false;
  std::string root_target = "root";
  bool find_echo = false;
  std::string echo_target = "echo";
  bool find_handler = false;
  std::string handler_target = "handler";
  bool find_location = false;
  std::string location_target = "location";
  bool find_static = false;
  std::string static_target = "static";
  bool find_status = false;
  std::string status_target = "status";
  bool find_proxy = false;
  std::string proxy_target = "proxy";
  bool find_proxyport = false;
  std::string proxyport_target = "proxyport";
  bool find_form = false;
  std::string form_target = "form";
  bool find_list = false;
  std::string list_target = "list";


  //bool inside_static = false;
  //bool inside_echo = false;

  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    if (find_port) {
      config->portnum = std::stoi(token);
      find_port = false;
    }
    if (find_root && !find_handler) {
      config->root_path = token;
      find_root = false;
    }
    if (find_static && find_location && find_handler) {
      config->client_access_static_.push_back(token);
      find_location = false;
    }
    if (find_static && find_root && find_handler) {
      config->static_path_.push_back(token);
      find_root = false;
    }
    if (find_proxy && find_location && find_handler) {
      config->client_access_proxy_.push_back(token);
      find_location = false;
    }
    if (find_proxy && find_root && find_handler) {
      config->proxy_path_.push_back(token);
      find_root = false;
    }
    if (find_proxy && find_proxyport && find_handler) {
      config->proxy_port_.push_back(token);
      find_proxyport = false;
    }
    if (find_echo && find_location && find_handler) {
      config->echo_path = token;
      find_location = false;
    }
    if (find_status && find_location && find_handler) {
      config->status_path = token;
      find_location = false;
    }
    if (find_form && find_location && find_handler) {
      config->upload_path = token;
      find_location = false;
    }
    if (find_form && find_list && find_handler) {
      config->list_path = token;
      find_list = false;
    }

   if(token.compare("}") == 0) {
      find_static = false;
      find_echo = false;
      find_handler = false;
      find_status = false;
      find_proxy = false;
      find_form = false;
    }
    if (token.compare(port_target) == 0) {
      find_port = true;
    }
    if (token.compare(root_target) == 0) {
      find_root= true;
    }
    if (token.compare(static_target) == 0) {
      find_static = true;
    }
    if (token.compare(echo_target) == 0) {
      find_echo = true;
    }
    if (token.compare(handler_target) == 0) {
      find_handler = true;
    }
    if (token.compare(location_target) == 0) {
      find_location = true;
    }
    if (token.compare(status_target) == 0) {
      find_status = true;
    }
    if (token.compare(proxy_target) == 0) {
      find_proxy = true;
    }
    if (token.compare(proxyport_target) == 0) {
      find_proxyport = true;
    }
    if (token.compare(form_target) == 0) {
      find_form = true;
    }
    if (token.compare(list_target) == 0) {
      find_list = true;
    }
    //printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL ||
               token_type == TOKEN_TYPE_QUOTED_STRING) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL ||
          last_token_type == TOKEN_TYPE_ENDOFLINE ||
          last_token_type == TOKEN_TYPE_QUOTED_STRING) { //revised
        if (last_token_type != TOKEN_TYPE_NORMAL &&
            last_token_type != TOKEN_TYPE_QUOTED_STRING) { //revised
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL &&
          last_token_type != TOKEN_TYPE_QUOTED_STRING) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_ENDOFLINE) {
        // Error.
        break;
      }
      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK &&
          last_token_type != TOKEN_TYPE_ENDOFLINE) {
        // Error.
        break;
      }
      return true;
    } else if (token_type == TOKEN_TYPE_ENDOFLINE) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK &&
          last_token_type != TOKEN_TYPE_START_BLOCK &&
          last_token_type != TOKEN_TYPE_ENDOFLINE &&
          last_token_type != TOKEN_TYPE_QUOTED_STRING) {
        //Error.
        break;
      }
    }
    else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  printf ("Bad transition from %s to %s\n",
          TokenTypeAsString(last_token_type),
          TokenTypeAsString(token_type));
  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    printf ("Failed to open config file: %s\n", file_name);
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}
