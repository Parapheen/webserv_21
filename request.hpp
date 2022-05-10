#pragma once

#include "response.hpp"
#include "ServerCfg.hpp"
#include "webserv.hpp"

struct config {
    std::string cgiExpansion;
};

class Request
{
private:
    std::string _method;
    std::string _uri;
    std::string _path;
    std::string _absPath;
    std::string _version;
    std::map<std::string, std::string> _headers;
    std::string _body;

    std::string _host;
    std::string _port;

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
    ServerCfg getConfig() const;

    void setMethod(std::string method);
    void setUri(std::string uri);
    void setVersion(std::string version);
    void setBody(std::string body);
    void setHeaders(std::map<std::string, std::string> headers);
    void setConfig(ServerCfg const& config);

    void initPort();
    Response parse(std::string message);
    bool getHeaders(std::string message);
    void parseUri();
    Response execute();
    Response execGet();
    Response execPost();
    Response execDelete();

    LocationCfg chooseLocation();
    // bool checkRediretion(LocationCfg const& location);
    //void printRequest(Request req);
    std::string getRequest();
};

std::ostream& operator<<(std::ostream &out, Request request);

std::string myToLower(std::string str);