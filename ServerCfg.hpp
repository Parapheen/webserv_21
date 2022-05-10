#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "LocationCfg.hpp"

class ServerCfg {
    private:
        int                         _port;
        std::string                 _name;
        std::string                 _root;
        // bool                        _autoIndex;
        // long long                   _clientBodyBufferSize;
        std::map<int, std::string>  _errorPages;
        std::vector<LocationCfg>    _locations;
    
    public:
        ServerCfg(void);
        ServerCfg(const int &port, const std::string &name, const std::string &root);
        ServerCfg(const ServerCfg &instance);

        ServerCfg   &operator=(const ServerCfg &rhs);

        ~ServerCfg(void);

        // const int                        &getPort(void) const;
        // const long long                  &getBodyBufferSize(void) const;
        // const std::string                &getName(void) const;
        // const std::string                &getRoot(void) const;
        // const bool                       &getAutoIndex(void) const;
        // const std::map<int, std::string> &getErrorPages(void) const;
        // const std::vector<LocationCfg>   &getLocations(void) const;

        int                        getPort(void) const;
        long long                  getBodyBufferSize(void) const;
        std::string                getName(void) const;
        std::string                getRoot(void) const;
        bool                       getAutoIndex(void) const;
        std::map<int, std::string> getErrorPages(void) const;
        std::vector<LocationCfg>   getLocations(void) const;

        void        setPort(const int &port);
        void        setBodyBufferSize(const long long &bodySize);
        void        setName(const std::string &name);
        void        setRoot(const std::string &root);
        void        setAutoIndex(const bool &autoIndex);
        void        addErrorPage(const int &statusCode, const std::string &path);
        void        addLocation(const LocationCfg &location);
};

std::ostream &operator<<(std::ostream &o, ServerCfg &instance);