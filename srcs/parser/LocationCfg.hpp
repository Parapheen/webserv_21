#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

enum METHOD {
    GET,
    POST,
    DELETE,
    PUT,
};

class LocationCfg {
    private:
        std::string                 _root;
        std::string                 _path;
        METHOD                      _method;
        bool                        _autoIndex;
        long long                   _clientBodyBufferSize;
        std::vector<std::string>    _index;
        std::string                 _cgiExpantion;
        std::string                 _redirectionCode;
        std::string                 _redirectionUrl;

    public:
        LocationCfg(void);
        LocationCfg(const LocationCfg &instance);

        LocationCfg &operator=(const LocationCfg &rhs);

        ~LocationCfg(void);

        const std::string               &getRoot(void) const;
        const std::string               &getPath(void) const;
        const METHOD                    &getMethod(void) const;
        const bool                      &getAutoIndex(void) const;
        const std::vector<std::string>  &getIndexes(void) const;
        const long long                 &getClientBodyBufferSize(void) const;
        const std::string               &getCgiExpantion(void) const;
        const std::string               &getRedirectionCode(void) const;
        const std::string               &getRedirectionUrl(void) const;

        void                            setRoot(const std::string &root);
        void                            setPath(const std::string &path);
        void                            setMethod(const std::string &method);
        void                            setAutoIndex(bool autoindex);
        void                            setIndexes(const std::vector<std::string> &indexes);
        void                            setClientBodyBufferSize(const long long &size);
        void                            setCgiExpantion(const std::string &expantion);
        void                            setRedirectionCode(const std::string &code);
        void                            setRedirectionUrl(const std::string &url);
};

std::ostream &operator<<(std::ostream &o, LocationCfg &instance);