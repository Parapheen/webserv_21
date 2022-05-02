#include "ServersManager.hpp"

ServersManager::ServersManager(const EventLoop &eventLoop) : Manager<TCPServer>(eventLoop) {
    return;
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

const TCPServer     *ServersManager::getServerBySocket(const int &fd) const {
    return this->getOne(fd);
}

void    ServersManager::removeSocket(const int &fd) {
    return this->removeOne(fd);
}

void    ServersManager::addServer(const std::string &serverName, TCPServer *server) {
    this->_servers.insert(std::pair<std::string, TCPServer *>(serverName, server));
    this->addSocket(server->getSocketFd(), server);
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