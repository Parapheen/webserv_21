#include "LocationCfg.hpp"

LocationCfg::LocationCfg(void) :
    _root("/"),
    _path("."),
    _method(GET),
    _autoIndex(false),
    _clientBodyBufferSize(10000000) { return; }

LocationCfg::LocationCfg(const LocationCfg &instance) : _root(instance.getRoot()), _path(instance.getPath()),
    _method(instance.getMethod()), _autoIndex(instance.getAutoIndex()), _clientBodyBufferSize(instance.getClientBodyBufferSize()),
    _index(instance.getIndexes()), _cgiExpantion(instance.getCgiExpantion()), _redirectionCode(instance.getRedirectionCode()),
    _redirectionUrl(instance.getRedirectionUrl()) {
    return ;
}

LocationCfg   &LocationCfg::operator=(const LocationCfg &rhs) {
    if (this != &rhs) {
        this->_autoIndex = rhs._autoIndex;
        this->_cgiExpantion = rhs._cgiExpantion;
        this->_clientBodyBufferSize = rhs._clientBodyBufferSize;
        this->_index = rhs._index;
        this->_method = rhs._method;
        this->_path = rhs._path;
        this->_redirectionCode = rhs._redirectionCode;
        this->_redirectionUrl = rhs._redirectionUrl;
        this->_root = rhs._root;
    }
    return *this;
}

const std::string               &LocationCfg::getPath(void) const { return this->_path; }
const std::string               &LocationCfg::getRoot(void) const { return this->_root; }
const METHOD                    &LocationCfg::getMethod(void) const { return this->_method; }
const bool                      &LocationCfg::getAutoIndex(void) const { return this->_autoIndex; };
const std::vector<std::string>  &LocationCfg::getIndexes(void) const { return this->_index; };
const long long                 &LocationCfg::getClientBodyBufferSize(void) const { return this->_clientBodyBufferSize; };
const std::string               &LocationCfg::getCgiExpantion(void) const { return this->_cgiExpantion; };
const std::string               &LocationCfg::getRedirectionCode(void) const { return this->_redirectionCode; };
const std::string               &LocationCfg::getRedirectionUrl(void) const { return this->_redirectionUrl; };

void                  LocationCfg::setPath(const std::string &path) { this->_path = path; }
void                  LocationCfg::setRoot(const std::string &root) { this->_root = root; }
void                  LocationCfg::setMethod(const std::string &method) {
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
    this->_method = _meth;
}
void                LocationCfg::setAutoIndex(bool autoindex) { this->_autoIndex = autoindex; };
void                LocationCfg::setIndexes(const std::vector<std::string> &indexes) { this->_index = indexes; };
void                LocationCfg::setClientBodyBufferSize(const long long &size) { this->_clientBodyBufferSize = size; };
void                LocationCfg::setCgiExpantion(const std::string &expantion) { this->_cgiExpantion = expantion; };
void                LocationCfg::setRedirectionCode(const std::string &code) { this->_redirectionCode = code; };
void                LocationCfg::setRedirectionUrl(const std::string &url) { this->_redirectionUrl = url; };


LocationCfg::~LocationCfg(void) { return; }

std::ostream    &operator<<(std::ostream &o, LocationCfg &instance) {
    o << "path: " << instance.getPath() << "\n"
    << "method: " << instance.getMethod() << "\n"
    << "root: " << instance.getRoot() << "\n";
    return o;
}