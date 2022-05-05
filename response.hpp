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
    std::string _version;
    std::string _statusCode;
    std::map<std::string, std::string> _headers;
    std::string _body;

    std::map<std::string, std::string> _statusMessages;

public:
    Response();
    Response(std::string const statusCode, std::string uri);
    //Response(std::string statusCode, int);
    Response(Response const& copy);
    Response& operator=(Response const& source);
    ~Response();

    void initStatusMessages();
    void initHeaders();

    void setHeaders(std::string lang, std::string uri = "\\"); //-
    
    void setDate(); //+
    void setContentType(std::string file); //+
    void setContentLenght(); //+
    void setContentLocation(std::string const& uri); //+
    void setContentLanguage(std::string const& lang = "en"); //+
    void setContentEncoding(); // ?????????
    void setAllow(); //+
    void setLastModified(std::string file); //+
    void setAcceptLanguage();
    void setAcceptEncoding();
    void setLocation(std::string uri); //+ Location используется для перенаправления и содержит новый URL адрес
    void setRetryAfter(); //+
    void setTransferEncoding(); //- chunked ??? 
    void setWwwAuthenticate(); //-
    void setVery(); // ????????

    void setBody(std::string body);

    void error(std::string statusCode);

    std::string getResponse();
};

std::ostream& operator<<(std::ostream &out, Response response);