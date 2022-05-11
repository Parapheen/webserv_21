#include "ClientsManager.hpp"

ClientsManager::ClientsManager(const EventLoop &eventLoop) : Manager(eventLoop) { return; }
ClientsManager::~ClientsManager(void) { 
    std::map<int, TCPServer *>::iterator it = this->_client_server_map.begin();
    while (it != this->_client_server_map.end()) {
        if (it->second)
            delete it->second;
        ++it;
    }
    return;
}

ClientsManager::ClientsManager(const ClientsManager &instance) : Manager(instance._eventLoop) { return; }
ClientsManager &ClientsManager::operator=(const ClientsManager &rhs) {
    if (this != &rhs) {
        this->_client_server_map = rhs._client_server_map; // iterate?
    }
    return *this;
}

const std::map<int, Client *> &ClientsManager::getClients(void) const { return this->getAll(); }

Client *ClientsManager::getClient(const int &fd) { return this->getOne(fd); }
TCPServer *ClientsManager::getServerByClientFd(const int &fd) { return this->_client_server_map[fd]; }

Client  *ClientsManager::createNewClient(const int &fd) {
    struct sockaddr_storage addr;
    struct kevent evSet;
    socklen_t socklen = sizeof(addr);
    
    memset(&evSet, 0, sizeof(evSet));
    int clientFd = accept(fd, (struct sockaddr *) &addr, &socklen);
    if (clientFd < 0)
        throw std::system_error(EFAULT, std::generic_category());
    EV_SET(&evSet, clientFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(this->_eventLoop.getEventLoop(), &evSet, 1, NULL, 0, NULL) == -1)
        throw std::system_error(EFAULT, std::generic_category());
    return new Client(clientFd);
}

void ClientsManager::addClient(TCPServer *server, Client *client) {
    this->addOne(client->getSocketFd(), client);
    this->_client_server_map[client->getSocketFd()] = server;
}


void ClientsManager::writeToClient(const int &fd) {
    struct kevent evSet;

    memset(&evSet, 0, sizeof(evSet));
    EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
    if (kevent(this->_eventLoop.getEventLoop(), &evSet, 1, NULL, 0, NULL) == -1)
        throw std::system_error(EFAULT, std::generic_category());
}

void ClientsManager::setResponseToClient(const int &fd, const Response &response) {
    this->getClient(fd)->setResponse(response);
}

const Response   &ClientsManager::getResponseByClientFd(const int &fd) {
    return this->getClient(fd)->getResponse();
}

void ClientsManager::removeClient(const int &fd) {
    struct kevent evSet;

    memset(&evSet, 0, sizeof(evSet));
    EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    if (kevent(this->_eventLoop.getEventLoop(), &evSet, 1, NULL, 0, NULL) == -1)
        throw std::system_error(EFAULT, std::generic_category());
    this->removeOne(fd);
    this->_client_server_map.erase(fd);
    close(fd);
}