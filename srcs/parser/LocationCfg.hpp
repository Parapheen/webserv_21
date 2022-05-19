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
        std::vector<METHOD>         _methods;
        bool                        _autoIndex;
        long long                   _clientBodyBufferSize;
        std::string                 _index;
        std::string                 _uploadDir;
        std::string                 _cgiExtention;
        std::string                 _cgiPath;
        std::string                 _redirectionCode;
        std::string                 _redirectionUrl;

    public:
        LocationCfg(void);
        LocationCfg(const LocationCfg &instance);

        LocationCfg &operator=(const LocationCfg &rhs);

        ~LocationCfg(void);

        const std::string               &getRoot(void) const;
        const std::string               &getPath(void) const;
        const std::vector<METHOD>       &getMethods(void) const;
        const bool                      &getAutoIndex(void) const;
        const std::string               &getIndex(void) const;
        const long long                 &getClientBodyBufferSize(void) const;
        const std::string               &getCgiExtention(void) const;
        const std::string               &getCgiPath(void) const;
        const std::string               &getRedirectionCode(void) const;
        const std::string               &getRedirectionUrl(void) const;
        const std::string               &getUploadDir(void) const;
        
        bool                            methodExists(const METHOD &method);

        void                            setRoot(const std::string &root);
        void                            setPath(const std::string &path);
        void                            addMethod(const std::string &method);
        void                            setAutoIndex(bool autoindex);
        void                            setIndex(const std::string &index);
        void                            setClientBodyBufferSize(const long long &size);
        void                            setCgiExtention(const std::string &extention);
        void                            setCgiPath(const std::string &cgiPath);
        void                            setRedirectionCode(const std::string &code);
        void                            setRedirectionUrl(const std::string &url);
        void                            setUploadDir(const std::string &uploadDir);
};

std::ostream &operator<<(std::ostream &o, LocationCfg &instance);