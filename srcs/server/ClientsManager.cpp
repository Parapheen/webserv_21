#include "ClientsManager.hpp"

ClientsManager::ClientsManager(const EventLoop &eventLoop) : Manager(eventLoop) { return; }
ClientsManager::~ClientsManager(void) { 
    std::map<int, const TCPServer *>::iterator it = this->_client_server_map.begin();
    while (it != this->_client_server_map.end()) {
        if (it->second)
            delete it->second;
        ++it;
    }
    return;
}

ClientsManager::ClientsManager(const ClientsManager &instance) : Manager(instance._eventLoop) { return; }

const std::map<int, Client *> &ClientsManager::getClients(void) const { return this->getAll(); }

const Client *ClientsManager::getClient(const int &fd) { return this->getOne(fd); }
const TCPServer *ClientsManager::getServerByClientFd(const int &fd) { return this->_client_server_map[fd]; }

Client  *ClientsManager::createNewClient(const int &fd) {
    struct sockaddr_storage addr;
    struct kevent evSet;
    socklen_t socklen = sizeof(addr);
    
    int clientFd = accept(fd, (struct sockaddr *) &addr, &socklen);
    if (clientFd < 0)
        throw std::system_error(EFAULT, std::generic_category());
    EV_SET(&evSet, clientFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(this->_eventLoop.getEventLoop(), &evSet, 1, NULL, 0, NULL);
    return new Client(clientFd);
}

void ClientsManager::addClient(const TCPServer *server, Client *client) {
    this->addOne(client->getSocketFd(), client);
    this->_client_server_map[client->getSocketFd()] = server;
}


void ClientsManager::removeClient(const int &fd) {
    struct kevent evSet;

    EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(this->_eventLoop.getEventLoop(), &evSet, 1, NULL, 0, NULL);
    this->removeOne(fd);
    this->_client_server_map.erase(fd);
    close(fd);
}