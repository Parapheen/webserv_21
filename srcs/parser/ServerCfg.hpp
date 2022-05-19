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
        std::map<std::string, std::string>  _errorPages;
        std::vector<LocationCfg>    _locations;
    
    public:
        ServerCfg(void);
        ServerCfg(const int &port, const std::string &name, const std::string &root);
        ServerCfg(const ServerCfg &instance);

        ServerCfg   &operator=(const ServerCfg &rhs);

        ~ServerCfg(void);

        const int                        &getPort(void) const;
        const std::string                &getName(void) const;
        const std::string                &getRoot(void) const;
        const std::map<std::string, std::string> &getErrorPages(void) const;
        const std::vector<LocationCfg>   &getLocations(void) const;

        void        setPort(const int &port);
        void        setName(const std::string &name);
        void        setRoot(const std::string &root);
        void        addErrorPage(const std::string &statusCode, const std::string &path);
        void        addLocation(const LocationCfg &location);
};

std::ostream &operator<<(std::ostream &o, ServerCfg &instance);