#include "Autoindex.hpp"

std::string Autoindex::createPage(std::string path, std::string host, std::string port)
{
    DIR *dpdf;
    struct dirent *epdf;
    std::string page = "<!DOCTYPE html>\
    <html>\
    <head>\
        <title>Index of " + path + "</title>\
    </head>\
    <body>";


    dpdf = opendir(path.c_str());
    if (dpdf != NULL)
    {
        while ((epdf = readdir(dpdf)))
            page += "\t<p><a href=\"http://" + host + ":" + port + "/" + std::string(epdf->d_name) + "\">" + std::string(epdf->d_name) + "</a></p>\n";
    }

    page += "</body>\n</html>";
    closedir(dpdf);
    return page;
};