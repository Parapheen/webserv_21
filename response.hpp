#pragma once

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <sys/stat.h>

class Response
{
private:
    std::string _version;
    std::string _statusCode;
    std::map<std::string, std::string> _headers;
    std::string _body;

    std::map<std::string, std::string> _statusMessages;

public:
    Response();
    Response(std::string const statusCode);
    //Response(std::string statusCode, int);
    Response(Response const& copy);
    Response& operator=(Response const& source);
    ~Response();

    void initStatusMessages();
    void initHeaders();

    void setHeaders(); //-
    void setDate(); //+
    void setContentType(std::string file); //+
    void setContentLenght(); //+
    void setContentEncoding(); //-
    void setContentLocation(); //-
    void setContentLanguage(); //-
    void setAllow(); //+
    void setLastModified(std::string file); //+
    void setAcceptLanguage();
    void setAcceptEncoding();
    void setLocation(std::string uri); //+-
    void setRetryAfter(); //+
    void setTransferEncoding(); //- chanked
    void setWWWAuthenticate(); //-

    void error(std::string statusCode);

    std::string getResponse();
};