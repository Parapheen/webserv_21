#pragma once
#include "../parser/ServerCfg.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"

class CgiHandler {
    private:
        std::string _binPath;

        ServerCfg   _config;
        LocationCfg _location;
        Request     _request;

    public:
        CgiHandler(void);
        CgiHandler(const CgiHandler &instance);

        CgiHandler  &operator=(const CgiHandler &rhs);

        ~CgiHandler(void);

        Response    getResponse(void);
};