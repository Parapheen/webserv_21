#include "Autoindex.hpp"

std::string Autoindex::createPage(const std::string &path, const std::string &uri) {
    DIR *dpdf;
    struct dirent *epdf;
    std::string page = "<!DOCTYPE html>\
    <html>\
    <head>\
        <title>Index of " + path + "</title>\
    </head>\
    <body>";

    std::string relativeUri = uri;
    if (relativeUri != "/")
        relativeUri += "/";
    dpdf = opendir(path.c_str());
    if (dpdf != NULL)
    {
        page += "\t<ul>";
        while ((epdf = readdir(dpdf)))
            page += "\t\t<li><a href=\"" + relativeUri + std::string(epdf->d_name) + "\">" + std::string(epdf->d_name) + "</a></li>\n";
        page += "\t</ul>";
    }

    page += "</body>\n</html>";
    closedir(dpdf);
    return page;
};