#include <algorithm>
#include <memory>
#include <sqlite3.h>
#include <vector>
#include <stack> 
#include <string>
#include <boost/lexical_cast.hpp>
#include "mimeHandler.h"

using namespace std;

mimeHandler::mimeHandler(const NginxConfig &config, const std::string &root_path)
{
  this->config = config;
  this->root_path = root_path;
}

mimeHandler* mimeHandler::create(const NginxConfig &config, const std::string &root_path)
{
  mimeHandler* handler = new mimeHandler(config, root_path);
  return handler;
}

unique_ptr<reply> mimeHandler::HandleRequest(const Request &request)
{
  if(request.uri == config.upload_path)
  {
    return view(request, 0);
  }
  else if(request.uri == config.list_path)
  {
    return list(request);
  }
  else if (request.uri == "/mime/new")
  {
    return edit(request);
  }
  else if(request.uri == "/mime/delete")
  {
    return del(request);
  }
}

//from https://stackoverflow.com/questions/14437433/proper-use-of-callback-function-of-sqlite3-in-c
int mimeHandler::read_db(int id)
{
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(config.db, "SELECT IMAGE, NAME, TEXT1, TEXT2 "
                                        "FROM IMAGES "
                                        "WHERE ID = ?", -1, &stmt, NULL);
  if (rc != SQLITE_OK)
  {
    return -2;
  }
  rc = sqlite3_bind_int(stmt, 1, id);
  if (rc != SQLITE_OK)
  {
    sqlite3_finalize(stmt);
    return -2;
  }

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW && rc != SQLITE_DONE)
  {
    sqlite3_finalize(stmt);
    return -2;
  }
  if (rc == SQLITE_DONE)
  {
    sqlite3_finalize(stmt);
    return -1;
  }
  int image_size = sqlite3_column_bytes(stmt, 0);
  char* img_ptr = (char*)sqlite3_column_blob(stmt, 0);
  vector<char> image_data(img_ptr, img_ptr+image_size);
  image = string(image_data.begin(), image_data.end());
  name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
  top = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
  bot = string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)));
  sqlite3_finalize(stmt);
  return 0;
}

int mimeHandler::get_last_id()
{
  sqlite3_stmt *pStmt;
  int rc;
  std::string tmp2 = "SELECT MAX(ID) FROM IMAGES";
  const char *idSql = tmp2.c_str();
  rc = sqlite3_prepare(config.db, idSql, -1, &pStmt, NULL);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  rc = sqlite3_step(pStmt);
  int last_id = sqlite3_column_int(pStmt, 0);
  rc = sqlite3_finalize(pStmt);
  return last_id;
}

unique_ptr<reply> mimeHandler::view(const Request &request, int flag) {
  unique_ptr<reply> response(new reply);
  string content;
  string id = request.file_name.substr(1);
  int ID = -1;
  try
  {
    ID = boost::lexical_cast<int>(id);
  }
  catch (boost::bad_lexical_cast)
  {
    response->set_header(400);
    response->set_length(0);
    return response;
  }
  string style = "<!DOCTYPE html>\n<html>\n<style>\nbody { display: inline-block; position: relative; }\nspan { color: white; font: 2em bold Impact, sans-serif; position: absolute; text-align: center; width: 100%; }\n#top { top: 0; }\n#bottom { bottom: 0; }\n</style>\n";
  
  // Inject the return value of read_db to state variable
  int state;
  if (flag != 0)
  {
    state = flag;
  }
  else {
    state = read_db(ID);
  }
  
  if (state == -1)
  {
    response->set_header(404);
    response->set_length(0);
    return response;
  }
  else if (state == -2)
  {
    response->set_header(503);
    response->set_length(0);
    return response;
  }

  encode(top);
  encode(bot);
  string body_up = "<body>\r\n";
  string html_image, html_top, html_bot;

  // Mock the output of the database
  if (flag != 0 )
  {
    html_image = "<img src=\"data:image/jpeg;base64,Donald Trump\">\r\n";
    html_top = "<span id=\"top\">Donald</span>\r\n";
    html_bot = "<span id=\"bottom\">Trump</span>\r\n";
  }
  else
  {
    html_image = "<img src=\"data:image/jpeg;base64," + image + "\">\r\n";
    html_top = "<span id=\"top\">" + top + "</span>\r\n";
    html_bot = "<span id=\"bottom\">" + bot + "</span>\r\n";
  }
  
  string body_bot = "</body>\r\n</html>";

  response->set_header(200);
  response->set_mime("text/html");
  response->set_length(style.length() + body_up.length() + html_image.length() + html_top.length() + html_bot.length() + body_bot.length());
  response->set_keep_alive(0, 0);
  response->set_content(style+body_up+html_image+html_top+html_bot+body_bot);
  
  return response;
}

unique_ptr<reply> mimeHandler::list(const Request &request) {
  unique_ptr<reply> response(new reply);
  std::string term;
  if(!(request.query.empty()))
  {
    term = request.query.substr(2);
  }
  std::stack<Item> s;
  string content;
  int last_id;
  last_id = get_last_id();
  int read = 0;
  if (last_id == 0) 
  {
    content = "No image in databse!";
    response->set_header(200);
    response->set_mime("text/html");
    response->set_length(content.size());
    response->set_content(content);
  }
  else if(last_id > 0)
  {
    content.append("<form method=\"get\">\n <input type=\"text\" name=\"q\" value=\""+term+"\">\n<input type=\"submit\" value=\"Search\">\n</form>");
    if(request.query.empty())
    {
      for(int id = 1; id <= last_id; id++)
      {
        read = read_db(id);
        if (read == -1)
        {
          continue;
        } 
        else if (read == -2)
        {
          continue;
        }
        encode(top);
        encode(bot);
        content.append( "<a href=\"" + config.upload_path + "?id=" + std::to_string(id) + "\">"+" id: "+std::to_string(id)+" top: "+top+" bot: "+bot+"</a>\n");
        content.append( "&nbsp<a href=\"/mime/new?update=" + std::to_string(id) + "\">Edit</a>");
        content.append( "&nbsp<a href=\"/mime/delete?id=" + std::to_string(id) + "\" onclick=\"return confirm('Are you sure?')\">Delete</a><br>");
      }
      response->set_header(200);
      response->set_mime("text/html");
      response->set_length(content.size());
      response->set_content(content);
    }
    else
    {
      content.append("<head>Search Results:</head><br>");
      bool find = false;
      for(int id = 1; id <= last_id; id++)
      {
        read = read_db(id);
        if (read == -1)
        {
          continue;
        } 
        else if (read == -2)
        {
          continue;          
        }
        encode(top);
        encode(bot);
        Item t = {id,name,top,bot,""};
        s.push(t);
        if(top.find(term)!=std::string::npos || bot.find(term)!=std::string::npos || name.find(term)!=std::string::npos)
        {
          find = true;
          content.append( "<a href=\"" + config.upload_path + "?id=" + std::to_string(id) + "\">"+" id: "+std::to_string(id)+ " image_name: " + name + +" top: "+top+" bot: "+bot+"</a>");
          content.append( "&nbsp<a href=\"/mime/new?update=" + std::to_string(id) + "\">Edit</a>");
          content.append( "&nbsp<a href=\"/mime/delete?id=" + std::to_string(id) + "\" onclick=\"return confirm('Are you sure?')\">Delete</a><br>");
        }
      }
      if(!find)
      {
        content.append("No Results!<br><br>");
      }
      content.append("<body>Reverse:</body><br>");
      while(!s.empty())
      {
        Item t = s.top();
        s.pop();
        content.append( "<a href=\"" + config.upload_path + "?id=" + std::to_string(t.id) + "\">"+" id: "+std::to_string(t.id)+ " image_name: " + t.name +" top: "+t.top+" bot: "+t.bot+"</a>");
        content.append( "&nbsp<a href=\"/mime/new?update=" + std::to_string(t.id) + "\">Edit</a>");
        content.append( "&nbsp<a href=\"/mime/delete?id=" + std::to_string(t.id) + "\" onclick=\"return confirm('Are you sure?')\">Delete</a><br>");
      }
      response->set_header(200);
      response->set_mime("text/html");
      response->set_length(content.size());
      response->set_content(content);
    }
  }
  else 
  {
    content = "Fail to get id! SQL error, rc = " + std::to_string(last_id);
    response->set_header(404);
    response->set_content(content);
  } 
  return response;
}

unique_ptr<reply> mimeHandler::edit(const Request &request) {
  unique_ptr<reply> response(new reply);
  string style = "<!DOCTYPE html>\n<html>";
  string id = request.file_name.substr(1);
  int ID = boost::lexical_cast<int>(id);
  int state = read_db(ID);
  if (state == -1)
  {
    response->set_header(404);
    response->set_length(0);
    return response;
  } else if (state == -2)
  {
    response->set_header(503);
    response->set_length(0);
    return response;
  }

  string body_up = "<body>\n";
  string form = "<form id=\"upload\" enctype=\"multipart/form-data\" action=\"\" method=\"post\">"
                  "<input id=\"image\" name=\"myimage\" type=\"file\"/><br>"
                  "<input type=\"text\" name=\"top\" value=\"" + top + "\"><br>"
                  "<input type=\"text\" name=\"bottom\" value=\"" + bot + "\"><br>"
                  "<input type=\"submit\" value=\"Update\" id=\"submit\">"
                "</form>";
  string body_bot = "</body>\n</html>";
  response->set_header(200);
  response->set_mime("text/html");
  response->set_length(style.length() + body_up.length() + form.length() + body_bot.length());
  response->set_keep_alive(0, 0);
  response->set_content(style+body_up+ form +body_bot);
  return response;
}

unique_ptr<reply> mimeHandler::del(const Request &request) {
  unique_ptr<reply> response(new reply);
  std::string content;
  std::string id = request.file_name.substr(1);
  int ID = -1;

  try
  {
    ID = boost::lexical_cast<int>(id);
  }
  catch (boost::bad_lexical_cast)
  {
    response->set_header(400);
    response->set_length(0);
    return response;
  }

  std::string tmp = "DELETE from IMAGES where ID = " + std::to_string(ID);
  const char *sql = tmp.c_str();
  char *err_msg = 0;
  int rc = sqlite3_exec(config.db, sql, 0, 0, &err_msg);

  if(rc != SQLITE_OK) {
    sqlite3_free(err_msg);
    response->set_header(503);
    response->set_length(0);
    return response;
  }

  std::string referer_host = "";
  for(auto header: request.headers)
  {
    std::transform(header.name.begin(), header.name.end(), header.name.begin(), ::tolower);
    if(header.name.compare("referer") == 0)
    {
      std::transform(header.value.begin(), header.value.end(), header.value.begin(), ::tolower);
      std::size_t found = header.value.find("/mime");
      referer_host = header.value.substr(0, found);
    }
  }

  content.append("<!DOCTYPE html>\r\n");
  content.append("<html>\r\n");
  content.append("  <head>\r\n");
  content.append("    <meta http-equiv=\"Refresh\" content=\"0; url=" + referer_host + "/mime/list\" />\r\n");
  content.append("  <head>\r\n");
  content.append("<html>\r\n");

  response->set_header(200);
  response->set_mime("text/html");
  response->set_length(content.size());
  response->set_content(content);

  return response;
}

void mimeHandler::encode(std::string &page)
{
    std::string buffer;
    buffer.reserve(page.size());
    for (size_t pos = 0; pos != page.size(); ++pos)
    {
        switch (page[pos])
        {
        case '&':
            buffer.append("&amp;");
            break;
        case '\"':
            buffer.append("&quot;");
            break;
        case '\'':
            buffer.append("&apos;");
            break;
        case '<':
            buffer.append("&lt;");
            break;
        case '>':
            buffer.append("&gt;");
            break;
        default:
            buffer.append(&page[pos], 1);
            break;
        }
    }
    page.swap(buffer);
}