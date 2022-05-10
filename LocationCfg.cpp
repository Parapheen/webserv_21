#include "LocationCfg.hpp"

LocationCfg::LocationCfg(void) { return; }

LocationCfg::LocationCfg(const LocationCfg &instance) {
    *this = instance;
    return ;
}

LocationCfg   &LocationCfg::operator=(const LocationCfg &rhs) {
    
    if (this != &rhs)
    {
        _root = rhs._root;
        _path = rhs._path;
        _method = rhs._method;
        _autoIndex = rhs._autoIndex;
        _clientBodyBufferSize = rhs._clientBodyBufferSize;
        _index = rhs._index;
        _cgiExpantion = rhs._cgiExpantion;
        _redirectionCode = rhs._redirectionCode;
        _redirectionUrl = rhs._redirectionUrl;
    }
    return *this;
}

std::string     LocationCfg::getPath(void) const { return this->_path; }
std::string     LocationCfg::getRoot(void) const { return this->_root; }
METHOD          LocationCfg::getMethod(void) const { return this->_method; }

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

void    LocationCfg::setAutoIndex(bool autoindex) { _autoIndex = autoindex; };
void    LocationCfg::setIndexes(std::vector<std::string> const& indexes) { _index = indexes; };
void    LocationCfg::setClientBodyBufferSize(long long const& size) { _clientBodyBufferSize = size; };
void    LocationCfg::setCgiExpantion(std::string const& expantion) { _cgiExpantion = expantion; };
void    LocationCfg::setRedirectionCode(std::string const& code) { _redirectionCode = code; };
void    LocationCfg::setRedirectionUrl(std::string const& url) { _redirectionUrl = url; };

bool LocationCfg::getAutoIndex(void) const
{
    return _autoIndex;
};

std::vector<std::string> LocationCfg::getIndexes(void) const
{
    return _index;
};

long long LocationCfg::getClientBodyBufferSize(void) const
{
    return _clientBodyBufferSize;
};

std::string LocationCfg::getCgiExpantion(void) const
{
    return _cgiExpantion;
};

std::string LocationCfg::getRedirectionCode() const
{
    return _redirectionCode;
};

std::string LocationCfg::getRedirectionUrl() const
{
    return _redirectionUrl;
};

LocationCfg::~LocationCfg(void) { return; }

std::ostream    &operator<<(std::ostream &o, LocationCfg &instance) {
    o << "path: " << instance.getPath() << "\n"
    << "method: " << instance.getMethod() << "\n"
    << "root: " << instance.getRoot() << "\n";
    return o;
}