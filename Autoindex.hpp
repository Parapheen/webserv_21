#pragma once

#include "webserv.hpp"

class Autoindex
{
    public:
        std::string createPage(std::string path, std::string host, std::string port);
};