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

        // const std::string   &getRoot(void) const;
        // const std::string   &getPath(void) const;
        // const METHOD        &getMethod(void) const;

        std::string                 getRoot(void) const;
        std::string                 getPath(void) const;
        METHOD                      getMethod(void) const;
        bool                        getAutoIndex(void) const;
        std::vector<std::string>    getIndexes(void) const;
        long long                   getClientBodyBufferSize(void) const;
        std::string                 getCgiExpantion(void) const;
        std::string                 getRedirectionCode() const;
        std::string                 getRedirectionUrl() const;

        void    setRoot(const std::string &root);
        void    setPath(const std::string &path);
        void    setMethod(const std::string &method);
        void    setAutoIndex(bool autoindex);
        void    setIndexes(std::vector<std::string> const& indexes);
        void    setClientBodyBufferSize(long long const& size = 0);
        void    setCgiExpantion(std::string const& expantion);
        void    setRedirectionCode(std::string const& code);
        void    setRedirectionUrl(std::string const& url);
};

std::ostream &operator<<(std::ostream &o, LocationCfg &instance);