#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include <unistd.h>

class Response
{
private:
    std::string                         _version;
    std::string                         _statusCode;
    std::map<std::string, std::string>  _headers;
    std::string                         _body;

    std::map<std::string, std::string>  _statusMessages;
    std::map<int, std::string>          _errorPages;

    std::string                         _cgiResponse; // remove if cgi will return str

public:
    Response(void);
    Response(std::string const& statusCode, std::map<int, std::string> const& errorPages, std::string const& uri);
    Response(Response const &copy);
    Response &operator=(Response const &source);
    ~Response(void);

    void initStatusMessages(void);
    void initHeaders(void);

    void setHeaders(const std::string &uri); //-
    void setDate(void);
    void setContentType(const std::string &file);
    void setContentLength(void);
    void setContentLocation(const std::string &uri);
    void setContentLanguage(const std::string &lang = "en");
    void setContentEncoding(void); // ?????????
    void setAllow(void);
    void setLastModified(const std::string &file);
    void setAcceptLanguage(void);
    void setAcceptEncoding(void);
    void setLocation(const std::string &uri); //+ Location используется для перенаправления и содержит новый URL адрес
    void setRetryAfter(void);
    void setTransferEncoding(void); //- chunked ??? 
    void setWwwAuthenticate(void); //-
    void setVery(void); // ????????

    void setBody(const std::string &body);

    void error(const std::string &statusCode);

    std::string getResponse(void);

    void setCgiResponse(std::string str); // remove if cgi will return str
};

std::ostream& operator<<(std::ostream &out, Response &response);