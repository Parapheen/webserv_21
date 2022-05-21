#pragma once

#include "Response.hpp"
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <dirent.h>
#include "../parser/ServerCfg.hpp"

enum REQUEST_STATE {
    EMPTY,
    IS_FIRST_LINE,
    IS_HEADERS,
    IS_BODY,
    DONE,
    ERROR,
    NOT_FULL_CHUNK
};

enum CHUNKED_STATE {
    NOT_CHUNKED,
    PROGRESS_CHUNKED,
    FINISH_CHUNKED
};

class Request {
private:
    std::string                         _rawRequest;
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
    LocationCfg                         _currentLocation;

    std::string                         _cgiResponse;
    REQUEST_STATE                       _status;
    std::string                         _error;
    std::string                         _parsingFirstLine;
    std::string                         _parsingHeaders;
    std::string                         _parsingBody;

    bool    _isDirectory(const char *path);
    Response _handleFormData(void);
    bool   _isEndingBoundary(const size_t &startPos, const size_t &boundarySize);
    std::string   _getFormDataBoundaryName(void);
    std::string  _getFormDataFileName(const size_t &offset);
    std::string  _getFormDataFileContent(const std::string &boundaryName, const size_t &offset);
    std::string _autoindex(void);
    std::string _indexFile(void);
    std::string _createHtmlPage(void);
    METHOD _strMethodToEnum(const std::string &method);

public:
    Request(void);
    Request(Request const& copy);
    Request& operator=(Request const& source);
    ~Request(void);

    const std::string                           &getMethod(void) const;
    const std::string                           &getUri(void) const;
    const std::string                           &getVersion(void) const;
    const std::string                           &getBody(void) const;
    const std::string                           &getAbsPath(void) const;
    const std::string                           &getRoot(const LocationCfg &location) const;
    const std::map<std::string, std::string>    &getHeaders(void) const;
    const ServerCfg                             &getConfig(void) const;
    bool                                        getHeaders(std::string message);
    const LocationCfg                           &getCurrentLocation(void) const;
    const std::string                           &getPath(void) const;

    void    setMethod(const std::string &method);
    void    setUri(const std::string &uri);
    void    setVersion(const std::string &version);
    void    setBody(const std::string &body);
    void    setHeaders(const std::map<std::string, std::string> &headers);
    void    setConfig(const ServerCfg &config);
    void    setRawRequest(const char *message, const int &len);

    REQUEST_STATE collectRequest(void);
    Response parse(void);
    bool isChunked(std::string const& buffer);
    void parseUri(void);
    Response execute(void);
    Response execGet(void);
    Response execPost(void);
    Response execDelete(void);

    std::string parseFirstLine(std::string firstLine);
    LocationCfg chooseLocation(void);
    void    constructAbsPath(bool isIndex);
    std::string getRequest(void);
    std::string nextLine(std::string const& text);
};

std::ostream& operator<<(std::ostream &out, Request &request);