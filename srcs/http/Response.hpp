#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <fstream>

class Response
{
private:
    std::string                         _version;
    std::string                         _cgi_response;

private:
    std::string                         _statusCode;
    std::map<std::string, std::string>  _headers;
    std::string                         _body;

    std::map<std::string, std::string>  _statusMessages;
    std::map<std::string, std::string>  _errorPages;

    bool    _hasDefaultErrorPage(const std::string &statusCode);

public:
    Response(void);
    Response(std::string const& statusCode, std::string const &uri, const std::map<std::string, std::string>  &errorPages);
    Response(Response const &copy);
    Response &operator=(Response const &source);
    ~Response(void);

    void initStatusMessages(void);
    void initHeaders(void);

    void setCgiResponse(const std::string &cgiResponse);
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

    std::string getResponse(void);
    const std::string &getCgiResponse() const;
};

std::ostream& operator<<(std::ostream &out, Response &response);