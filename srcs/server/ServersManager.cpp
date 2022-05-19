#include "ServersManager.hpp"

ServersManager::ServersManager(void) { return ; }

ServersManager::ServersManager(const EventLoop &eventLoop) : Manager<TCPServer>(eventLoop) {
    return;
}

ServersManager::ServersManager(const ServersManager &instance) : _servers(instance._servers) { return; }

ServersManager &ServersManager::operator=(const ServersManager &rhs) {
    if (this != &rhs) {
        this->_servers = rhs._servers;
    }
    return *this;
}

const std::map<std::string, TCPServer *> &ServersManager::getServers(void) const {
    return this->_servers;
}

void    ServersManager::addSocket(const int &fd, TCPServer *server) {
    this->addOne(fd, server);
    return ;
}

const std::map<int, TCPServer *>    &ServersManager::getSockets(void) const {
    return this->getAll();
}

TCPServer     *ServersManager::getServerBySocket(const int &fd) { return this->getOne(fd); }

void    ServersManager::removeSocket(const int &fd) {
    return this->removeOne(fd);
}

void    ServersManager::addServer(const std::string &serverName, TCPServer *server) {
    this->_servers.insert(std::pair<std::string, TCPServer *>(serverName, server));
    this->addSocket(server->getSocketFd(), server);
}

void ServersManager::init(const std::vector<ServerCfg> &servers) {
    for(size_t i = 0; i < servers.size(); i++) {
        TCPServer *newServ = new TCPServer(this->_eventLoop.getEventLoop(), servers[i]);
        this->addServer(servers[i].getName(), newServ);
    }
}

ServersManager::~ServersManager(void) {
    std::map<std::string, TCPServer *>::iterator it = this->_servers.begin();
    while (it != this->_servers.end()) {
        if (it->second)
            delete it->second;
        ++it;
    }
    return;
}