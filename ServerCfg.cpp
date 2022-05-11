#include "ServerCfg.hpp"

ServerCfg::ServerCfg(void) : _port(8000), _name(""), _root("/") { return; }

ServerCfg::ServerCfg(const ServerCfg &instance) {
    *this = instance;
    return ;
}

ServerCfg   &ServerCfg::operator=(const ServerCfg &rhs) {
    // if (this->getPort() != rhs.getPort())
    //     this->setPort(rhs.getPort());
    // else if (this->getRoot() != rhs.getRoot())
    //     this->setRoot(rhs.getRoot());
    // else if (this->getName() != rhs.getName())
    //     this->setName(rhs.getName());
    
    if (this != &rhs)
    {
        _root = rhs._port;
        _name = rhs._name;
        _root = rhs._port;
        _errorPages = rhs._errorPages;
        _locations = rhs._locations;
    }
    return *this;
}

int                           ServerCfg::getPort(void) const { return this->_port; }
//long long                     ServerCfg::getBodyBufferSize(void) const { return this->_clientBodyBufferSize; }
std::string                   ServerCfg::getRoot(void) const { return this->_root; }
//bool                          ServerCfg::getAutoIndex(void) const { return this->_autoIndex; }
std::string                   ServerCfg::getName(void) const { return this->_name; }
std::map<int, std::string>    ServerCfg::getErrorPages(void) const { return this->_errorPages; }
std::vector<LocationCfg>      ServerCfg::getLocations(void) const { return this->_locations; }

void                                ServerCfg::setPort(const int &port) { this->_port = port; }
//void                                ServerCfg::setBodyBufferSize(const long long &bufferSize) { this->_clientBodyBufferSize = bufferSize; }
void                                ServerCfg::setRoot(const std::string &root) { this->_root = root; }
//void                                ServerCfg::setAutoIndex(const bool &autoIndex) { this->_autoIndex = autoIndex; }
void                                ServerCfg::setName(const std::string &name) { this->_name = name; }
void                                ServerCfg::addErrorPage(const int &statusCode, const std::string &path) { this->_errorPages[statusCode] = path; }
void                                ServerCfg::addLocation(const LocationCfg &location) { this->_locations.push_back(location); }

ServerCfg::~ServerCfg(void) { return; }

std::ostream    &operator<<(std::ostream &o, ServerCfg &instance) {
    o << "server_name: " << instance.getName() << "\n"
    << "port: " << instance.getPort() << "\n"
    << "root: " << instance.getRoot() << "\n"
    // << "client_body_buffer_size: " << instance.getBodyBufferSize() << "\n"
    // << "autoindex: " << instance.getAutoIndex() << "\n"
    << "error_pages length: " << instance.getErrorPages().size() << "\n"
    << "locations length: " << instance.getLocations().size();
    return o;
}