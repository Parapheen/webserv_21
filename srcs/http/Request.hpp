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
    std::string                         _path;
    std::string                         _absPath;
    std::string                         _version;
    std::map<std::string, std::string>  _headers;
    std::string                         _body;

    std::string                         _host;
    std::string                         _port;

    ServerCfg                           _conf;

public:
    Request(void);
    Request(Request const& copy);
    Request& operator=(Request const& source);
    ~Request(void);

    const std::string                           &getMethod(void) const;
    const std::string                           &getUri(void) const;
    const std::string                           &getVersion(void) const;
    const std::string                           &getBody(void) const;
    const std::map<std::string, std::string>    &getHeaders(void) const;
    const ServerCfg                             &getConfig(void) const;
    bool                                        getHeaders(std::string message);

    void    setMethod(const std::string &method);
    void    setUri(const std::string &uri);
    void    setVersion(const std::string &version);
    void    setBody(const std::string &body);
    void    setHeaders(const std::map<std::string, std::string> &headers);
    void    setConfig(const ServerCfg &config);

    Response parse(const std::string &message);
    void    initPort(void);
    void parseUri(void);
    Response execute(void);
    Response execGet(void);
    Response execPost(void);
    Response execDelete(void);

    LocationCfg chooseLocation(void);
    //void printRequest(Request req);
    // bool checkRediretion(LocationCfg const& location);
    std::string getRequest(void);

};

std::ostream& operator<<(std::ostream &out, Request &request);

std::string myToLower(const std::string &str);