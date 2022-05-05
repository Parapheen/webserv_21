#pragma once

#include "response.hpp"
#include <string>
#include <map>
#include <fstream>
#include <cstdio>
#include "ServerCfg.hpp"

struct config {
    std::string cgiExpansion;
};

class Request
{
private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;

    ServerCfg _conf;

public:
    Request();
    Request(Request const& copy);
    Request& operator=(Request const& source);
    ~Request();

    std::string getMethod() const;
    std::string getUri() const;
    std::string getVersion() const;
    std::string getBody() const;
    std::map<std::string, std::string> getHeaders() const;

    void setMethod(std::string method);
    void setUri(std::string uri);
    void setVersion(std::string version);
    void setBody(std::string body);
    void setHeaders(std::map<std::string, std::string> headers);

    Response parse(std::string message);
    bool getHeaders(std::string message);
    void parseUri();
    Response execute();
    Response execGet();
    Response execPost();
    Response execDelete();

    //void printRequest(Request req);
    std::string getRequest();

    void setConfig(ServerCfg config);
    ServerCfg getConfig() const;
};

std::ostream& operator<<(std::ostream &out, Request request);

std::string myToLower(std::string str);