#include <stdio.h>
#include <string>
#include "formHandler.h"
#include <boost/lexical_cast.hpp>

using namespace std;

formHandler::formHandler(const NginxConfig& config, const std::string& root_path) 
{
	this->config = config;
	this->root_path = root_path;
}

formHandler* formHandler::create(const NginxConfig& config, const string& root_path)
{
    formHandler* form_handler = new formHandler(config, root_path);
    return form_handler;
}

void formHandler::get_form_content(const Request* req, std::string* top, std::string* bottom, std::string* image, std::string* name)
{
    std::string data(req->content.begin(),req->content.end());
    std::string block;
    std::string boundary;
    std::size_t boundary_size = boundary.size() + 1;
    std::size_t boundary_end = data.find_first_of("\n");
    bool flag = false;
    int status = -1;
    boundary = data.substr(0,boundary_end-1);
    vector<size_t> boundary_pos;

    size_t pos = data.find(boundary, 0);
    while(pos != string::npos)
    {
        boundary_pos.push_back(pos);
        pos = data.find(boundary,pos+1);
    }
    std::size_t start;
    std::size_t name_idx;
    for(int i = 0; i < 3; i++)
    {
        block = data.substr(boundary_pos.at(i), boundary_pos.at(i+1)-boundary_pos.at(i)-boundary_size-1);
        status = check_content_type(&block);
        switch(status)
        {
            case 0:
                start = block.find("name=\"image\"")+12+4;
                *name = block.substr(start,std::string::npos);
                *image = block.substr(start,std::string::npos);
                break;
            case 1:
                start = block.find("Content-Type:");
                name_idx = block.find("filename=");
                *name = block.substr(name_idx+10, start-3-(name_idx+10));
                block = block.substr(start,std::string::npos);
                start = block.find_first_of("\n");
                *image = block.substr(start+3,std::string::npos);
                break;
            case 2:
                start = block.find("name=\"top\"")+10+4;
                *top = block.substr(start,std::string::npos);
                break;
            case 3:
                start = block.find("name=\"bottom\"")+13+4;
                *bottom = block.substr(start,std::string::npos);
                break;
            default:
                std::cerr << "Error" << std::endl;
        }
    }
}

int formHandler::insert_table(std::string &image, std::string name, std::string top, std::string bottom)
{
  const char *zBlob = image.c_str();
  int nBlob = image.length();
  std::string tmp = "INSERT INTO IMAGES(IMAGE,NAME,TEXT1,TEXT2) VALUES(?1, ?2, ?3, ?4)";
  const char *zSql = tmp.c_str();
  sqlite3_stmt *pStmt;
  int rc;
  char *zErrMsg = 0;
  const char* data = "Get_ID function called";

  do {
    rc = sqlite3_prepare(config.db, zSql, -1, &pStmt, 0);
    if( rc!=SQLITE_OK ){
      return rc;
    }
    rc = sqlite3_bind_blob(pStmt, 1, zBlob, nBlob, SQLITE_STATIC);
    if( rc!=SQLITE_OK ){
        return rc;
    }
    
    rc = sqlite3_bind_text(pStmt, 2, name.c_str(), name.length(), SQLITE_STATIC);
    if( rc!=SQLITE_OK ){
        return rc;
    }
    rc = sqlite3_bind_text(pStmt, 3, top.c_str(), top.length(), SQLITE_STATIC);
    if( rc!=SQLITE_OK ){
        return rc;
    }
    rc = sqlite3_bind_text(pStmt, 4, bottom.c_str(), bottom.length(), SQLITE_STATIC);
    if( rc!=SQLITE_OK ){
        return rc;
    }
    rc = sqlite3_step(pStmt);
    assert( rc!=SQLITE_ROW );
    rc = sqlite3_finalize(pStmt);
  } while( rc==SQLITE_SCHEMA );

  std::string tmp2 = "SELECT last_insert_rowid() FROM IMAGES";
  const char *idSql = tmp2.c_str();
  rc = sqlite3_prepare(config.db, idSql, -1, &pStmt, 0);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  rc = sqlite3_step(pStmt);
  last_id = sqlite3_column_int(pStmt, 0);
  rc = sqlite3_finalize(pStmt);
  return rc;
}

int formHandler::update_table(std::string &image, std::string name, std::string top, std::string bottom) {
    std::string stmt;
    sqlite3_stmt *pStmt;
    int rc;

    if (!image.empty()) {
        stmt = "UPDATE IMAGES SET IMAGE = ?1, NAME = ?2 WHERE ID = " + std::to_string(curr_id);
        const char* cstmt = stmt.c_str();
        rc = sqlite3_prepare(config.db, cstmt, -1, &pStmt, 0);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        rc = sqlite3_bind_blob(pStmt, 1, image.c_str(), image.length(), SQLITE_STATIC);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        rc = sqlite3_bind_text(pStmt, 2, name.c_str(), name.length(), SQLITE_STATIC);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        rc = sqlite3_step(pStmt);
        assert( rc!=SQLITE_ROW );
        rc = sqlite3_finalize(pStmt);
    }
    if (!top.empty()) {
        stmt = "UPDATE IMAGES SET TEXT1 = ?3 WHERE ID = " + std::to_string(curr_id);
        const char* cstmt = stmt.c_str();
        rc = sqlite3_prepare(config.db, cstmt, -1, &pStmt, 0);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        rc = sqlite3_bind_text(pStmt, 3, top.c_str(), top.length(), SQLITE_STATIC);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        rc = sqlite3_step(pStmt);
        assert( rc!=SQLITE_ROW );
        rc = sqlite3_finalize(pStmt);
    }
    if (!bottom.empty()) {
        stmt = "UPDATE IMAGES SET TEXT2 = ?4 WHERE ID = " + std::to_string(curr_id);
        const char* cstmt = stmt.c_str();
        rc = sqlite3_prepare(config.db, cstmt, -1, &pStmt, 0);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        rc = sqlite3_bind_text(pStmt, 4, bottom.c_str(), bottom.length(), SQLITE_STATIC);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        rc = sqlite3_step(pStmt);
        assert( rc!=SQLITE_ROW );
        rc = sqlite3_finalize(pStmt);
    }
    return rc;
}

std::unique_ptr<reply> formHandler::HandleRequest(const Request& request)
{
    std::string top;
    std::string bottom;
    std::string image;
    std::string name;
    std::unique_ptr<reply> response(new reply);
	string content;
    std::string bad = "<script>";

    get_form_content(&request,&top, &bottom, &image, &name);
    // encode(top);
    // encode(bottom);
    // if(top.find(bad) != std::string::npos || bottom.find(bad) != std::string::npos)
    // {
    //     content = "Malicious texts!";
    //     response->set_header(503);
    //     response->set_content(content);
    //     return response;
    // }

    if(image == "powell.jpg" || image == "ucla.jpg")
    {
        ostringstream out;
        std::string path = config.root_path+config.static_path_.at(0)+"/"+image;
        ifstream file(path, std::ios::binary);

        out << file.rdbuf();
        image = out.str();
    }
    auto encoded = cppcodec::base64_rfc4648::encode(image);
    image = std::string(encoded.begin(), encoded.end());

    int idPos = request.request_uri.find('?', 1);
    int rc;
    if (idPos == std::string::npos) {
        rc = insert_table(image, name, top, bottom);
        if (rc != SQLITE_OK) {
            content = "Fail to create Mime! SQL error, rc = " + std::to_string(rc);
            response->set_header(404);
            response->set_content(content);
        } else {
            content = "Created mime!\n<a href=\"" + config.upload_path + "?id=" + std::to_string(last_id) + "\">id="+std::to_string(last_id)+"</a>";
            response->set_header(200);
            response->set_mime("text/html");
            response->set_length(content.size());
            response->set_content(content);
        }
    }
    else {
        std::string id = request.request_uri.substr(idPos+8);
        try
        {
            curr_id = boost::lexical_cast<int>(id);
        }
        catch (boost::bad_lexical_cast)
        {
            response->set_header(400);
            response->set_length(0);
            return response;
        }
        rc = update_table(image, name, top, bottom);
        if (rc != SQLITE_OK) {
            content = "Fail to Update Mime! SQL error, rc = " + std::to_string(rc);
            response->set_header(404);
            response->set_content(content);
        } else {
            content = "Updated meme!\n<a href=\"" + config.upload_path + "?id=" + std::to_string(curr_id) + "\">id="+std::to_string(curr_id)+"</a>";
            response->set_header(200);
            response->set_mime("text/html");
            response->set_length(content.size());
            response->set_content(content);
        }
    }
    return response;
}

int formHandler::check_content_type(std::string* content)
{
    if(content->find("name=\"image\"") != std::string::npos)
        return 0; // image
    if(content->find("filename=") != std::string::npos)
        return 1; // image_upload
    if(content->find("name=\"top\"") != std::string::npos)
        return 2; // top
    if(content->find("name=\"bottom\"") != std::string::npos)
        return 3; // bot
    return -1;
}
/*
//from https://stackoverflow.com/questions/5665231/most-efficient-way-to-escape-xml-html-in-c-string
void formHandler::encode(std::string &page)
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
*/