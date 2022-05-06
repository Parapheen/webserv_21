#pragma once

#include "Response.hpp"
#include <string>
#include <map>
#include <fstream>
#include <cstdio>
#include "../parser/ServerCfg.hpp"

struct config {
    std::string cgiExpansion;
};

class Request
{
private:
    std::string                         _method;
    std::string                         _uri;
    std::string                         _version;
    std::map<std::string, std::string>  _headers;
    std::string                         _body;

    ServerCfg                           _conf;

public:
    Request(void);
    Request(Request const& copy);
    Request& operator=(Request const& source);
    ~Request(void);

    const std::string                           &getMethod() const;
    const std::string                           &getUri() const;
    const std::string                           &getVersion() const;
    const std::string                           &getBody() const;
    const std::map<std::string, std::string>    &getHeaders() const;

    void setMethod(const std::string &method);
    void setUri(const std::string &uri);
    void setVersion(const std::string &version);
    void setBody(const std::string &body);
    void setHeaders(const std::map<std::string, std::string> &headers);

    Response parse(const std::string &message);
    bool getHeaders(std::string message);
    void parseUri(void);
    Response execute(void);
    Response execGet(void);
    Response execPost(void);
    Response execDelete(void);

    //void printRequest(Request req);
    std::string getRequest(void);

    void setConfig(ServerCfg config);
    const ServerCfg &getConfig(void) const;
};

std::ostream& operator<<(std::ostream &out, Request &request);

std::string myToLower(const std::string &str);