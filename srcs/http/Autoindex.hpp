#pragma once
#include <string>
#include <dirent.h>

class Autoindex {
    public:
        std::string createPage(const std::string &path, const std::string &uri);
};