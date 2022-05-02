#include "LocationCfg.hpp"

LocationCfg::LocationCfg(void) { return; }

LocationCfg::LocationCfg(const LocationCfg &instance) {
    *this = instance;
    return ;
}

LocationCfg   &LocationCfg::operator=(const LocationCfg &rhs) {
    if (this->getPath() != rhs.getPath())
        this->setPath(rhs.getPath());
    else if (this->getRoot() != rhs.getRoot())
        this->setRoot(rhs.getRoot());
    return *this;
}

const std::string     &LocationCfg::getPath(void) const { return this->_path; }
const std::string     &LocationCfg::getRoot(void) const { return this->_root; }
const METHOD          &LocationCfg::getMethod(void) const { return this->_method; }

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

LocationCfg::~LocationCfg(void) { return; }

std::ostream    &operator<<(std::ostream &o, LocationCfg &instance) {
    o << "path: " << instance.getPath() << "\n"
    << "method: " << instance.getMethod() << "\n"
    << "root: " << instance.getRoot() << "\n";
    return o;
}