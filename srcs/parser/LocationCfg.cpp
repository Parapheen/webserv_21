#include "LocationCfg.hpp"

LocationCfg::LocationCfg(void) :
    _root("/"),
    _path("."),
    _methods(std::vector<METHOD>(GET)),
    _autoIndex(false),
    _clientBodyBufferSize(10000000) { return; }

LocationCfg::LocationCfg(const LocationCfg &instance) : _root(instance.getRoot()), _path(instance.getPath()),
    _methods(instance.getMethods()), _autoIndex(instance.getAutoIndex()), _clientBodyBufferSize(instance.getClientBodyBufferSize()),
    _index(instance.getIndex()), _cgiExtention(instance.getCgiExtention()), _cgiPath(instance.getCgiPath()), _redirectionCode(instance.getRedirectionCode()),
    _redirectionUrl(instance.getRedirectionUrl()) {
    return ;
}

LocationCfg   &LocationCfg::operator=(const LocationCfg &rhs) {
    if (this != &rhs) {
        this->_autoIndex = rhs._autoIndex;
        this->_cgiExtention = rhs._cgiExtention;
        this->_cgiPath = rhs._cgiPath;
        this->_clientBodyBufferSize = rhs._clientBodyBufferSize;
        this->_index = rhs._index;
        this->_methods = rhs._methods;
        this->_path = rhs._path;
        this->_redirectionCode = rhs._redirectionCode;
        this->_redirectionUrl = rhs._redirectionUrl;
        this->_root = rhs._root;
    }
    return *this;
}

const std::string               &LocationCfg::getPath(void) const { return this->_path; }
const std::string               &LocationCfg::getRoot(void) const { return this->_root; }
const std::vector<METHOD>       &LocationCfg::getMethods(void) const { return this->_methods; }
const bool                      &LocationCfg::getAutoIndex(void) const { return this->_autoIndex; };
const std::string               &LocationCfg::getIndex(void) const { return this->_index; };
const long long                 &LocationCfg::getClientBodyBufferSize(void) const { return this->_clientBodyBufferSize; };
const std::string               &LocationCfg::getCgiExtention(void) const { return this->_cgiExtention; };
const std::string               &LocationCfg::getCgiPath(void) const { return this->_cgiPath; };
const std::string               &LocationCfg::getRedirectionCode(void) const { return this->_redirectionCode; };
const std::string               &LocationCfg::getRedirectionUrl(void) const { return this->_redirectionUrl; };
const std::string               &LocationCfg::getUploadDir(void) const { return this->_uploadDir; };

void                  LocationCfg::setPath(const std::string &path) { this->_path = path; }
void                  LocationCfg::setRoot(const std::string &root) { this->_root = root; }
void                  LocationCfg::addMethod(const std::string &method) {
    METHOD _meth;
    if (method == "GET")
        _meth = GET;
    else if (method == "POST")
        _meth = POST;
    else if (method == "PUT")
        _meth = PUT;
    else if (method == "DELETE")
        _meth = DELETE;
    else {
        std::ostringstream errMsg;
        errMsg << "Parser error\nInvalid method in location: '" << method << "'";
        throw std::runtime_error(errMsg.str());
    }
    this->_methods.push_back(_meth);
}
void                LocationCfg::setAutoIndex(bool autoindex) { this->_autoIndex = autoindex; };
void                LocationCfg::setIndex(const std::string &index) { this->_index = index; };
void                LocationCfg::setClientBodyBufferSize(const long long &size) { this->_clientBodyBufferSize = size; };
void                LocationCfg::setCgiExtention(const std::string &extention) { this->_cgiExtention = extention; };
void                LocationCfg::setCgiPath(const std::string &cgiPath) { this->_cgiPath = cgiPath; };
void                LocationCfg::setRedirectionCode(const std::string &code) { this->_redirectionCode = code; };
void                LocationCfg::setRedirectionUrl(const std::string &url) { this->_redirectionUrl = url; };
void                LocationCfg::setUploadDir(const std::string &uploadDir) { this->_uploadDir = uploadDir; };


LocationCfg::~LocationCfg(void) { return; }

std::ostream    &operator<<(std::ostream &o, LocationCfg &instance) {
    o << "path: " << instance.getPath() << "\n"
    << "root: " << instance.getRoot() << "\n";
    return o;
}