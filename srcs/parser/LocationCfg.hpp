#pragma once
#include <string>
#include <iostream>
#include <sstream>

enum METHOD {
    GET,
    POST,
    DELETE,
    PUT,
};

class LocationCfg {
    private:
        std::string _root;
        std::string _path;
        METHOD      _method;
    
    public:
        LocationCfg(void);
        LocationCfg(const LocationCfg &instance);

        LocationCfg &operator=(const LocationCfg &rhs);

        ~LocationCfg(void);

        const std::string   &getRoot(void) const;
        const std::string   &getPath(void) const;
        const METHOD        &getMethod(void) const;

        void    setRoot(const std::string &root);
        void    setPath(const std::string &path);
        void    setMethod(const std::string &method);
};

std::ostream &operator<<(std::ostream &o, LocationCfg &instance);