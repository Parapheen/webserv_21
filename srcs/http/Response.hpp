#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <sys/stat.h>

class Response
{
private:
    //const static std::string _version = "HTTP/1.1";
    std::string                         _version;
    std::string                         _statusCode;
    std::map<std::string, std::string>  _headers;
    std::string                         _body;

    std::map<std::string, std::string>  _statusMessages;

public:
    Response(void);
    Response(std::string const &statusCode, const std::string &uri);
    //Response(std::string statusCode, int);
    Response(Response const &copy);
    Response &operator=(Response const &source);
    ~Response(void);

    void initStatusMessages(void);
    void initHeaders(void);

    void setHeaders(const std::string &lang, const std::string &uri = "\\"); //-
    
    void setDate(void); //+
    void setContentType(const std::string &file); //+
    void setContentLenght(void); //+
    void setContentLocation(const std::string &uri); //+
    void setContentLanguage(const std::string &lang = "en"); //+
    void setContentEncoding(void); // ?????????
    void setAllow(void); //+
    void setLastModified(const std::string &file); //+
    void setAcceptLanguage(void);
    void setAcceptEncoding(void);
    void setLocation(const std::string &uri); //+ Location используется для перенаправления и содержит новый URL адрес
    void setRetryAfter(void); //+
    void setTransferEncoding(void); //- chunked ??? 
    void setWwwAuthenticate(void); //-
    void setVery(void); // ????????

    void setBody(const std::string &body);

    void error(const std::string &statusCode);

    std::string getResponse(void);
};

std::ostream& operator<<(std::ostream &out, Response &response);