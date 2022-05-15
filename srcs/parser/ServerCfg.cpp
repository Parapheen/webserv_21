#include "ServerCfg.hpp"

ServerCfg::ServerCfg(void) : _port(8000), _name(""), _root("/"), _errorPages(std::map<int, std::string>()),
    _locations(std::vector<LocationCfg>()) {
    this->_errorPages[500] = "www/error_pages/500.html";
    this->_errorPages[404] = "www/error_pages/404.html";
    return;
}

ServerCfg::ServerCfg(const ServerCfg &instance) : _port(instance.getPort()), _name(instance.getName()),
    _root(instance.getRoot()), _errorPages(instance.getErrorPages()), _locations(instance.getLocations()) {
    return ;
}

ServerCfg   &ServerCfg::operator=(const ServerCfg &rhs) {
    if (this == &rhs) return *this;
    this->_port = rhs._port;
    this->_name = rhs._name;
    this->_root = rhs._root;
    this->_errorPages = rhs._errorPages;
    this->_locations = rhs.getLocations();
    return *this;
}

const int                           &ServerCfg::getPort(void) const { return this->_port; }
const std::string                   &ServerCfg::getRoot(void) const { return this->_root; }
const std::string                   &ServerCfg::getName(void) const { return this->_name; }
const std::map<int, std::string>    &ServerCfg::getErrorPages(void) const { return this->_errorPages; }
const std::vector<LocationCfg>      &ServerCfg::getLocations(void) const { return this->_locations; }

void                                ServerCfg::setPort(const int &port) { this->_port = port; }
void                                ServerCfg::setRoot(const std::string &root) { this->_root = root; }
void                                ServerCfg::setName(const std::string &name) { this->_name = name; }
void                                ServerCfg::addErrorPage(const int &statusCode, const std::string &path) { this->_errorPages[statusCode] = path; }
void                                ServerCfg::addLocation(const LocationCfg &location) { this->_locations.push_back(location); }

ServerCfg::~ServerCfg(void) { return; }

std::ostream    &operator<<(std::ostream &o, ServerCfg &instance) {
    o << "server_name: " << instance.getName() << "\n"
    << "port: " << instance.getPort() << "\n"
    << "root: " << instance.getRoot() << "\n"
    << "error_pages length: " << instance.getErrorPages().size() << "\n"
    << "locations length: " << instance.getLocations().size();
    return o;
}