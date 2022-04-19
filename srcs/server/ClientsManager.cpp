#include "ClientsManager.hpp"

ClientsManager::ClientsManager(const EventLoop &eventLoop) : Manager(eventLoop) { return; }
ClientsManager::~ClientsManager(void) { return; }

ClientsManager::ClientsManager(const ClientsManager &instance) : Manager(instance._eventLoop) { return; }

const std::map<int, Client *> &ClientsManager::getClients(void) const {
    return this->getAll();
}

const Client *ClientsManager::getClient(const int &fd) {
    return this->getOne(fd);
}

Client  *ClientsManager::createNewClient(const int &fd) {
    struct sockaddr_storage addr;
    struct kevent evSet;
    socklen_t socklen = sizeof(addr);
    
    int clientFd = accept(fd, (struct sockaddr *) &addr, &socklen);
    if (clientFd < 0)
        throw std::system_error(EFAULT, std::generic_category());
    EV_SET(&evSet, clientFd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(this->_eventLoop.getEventLoop(), &evSet, 1, NULL, 0, NULL);
    char msg[80];
    sprintf(msg, "welcome! you are connected to socket %d!\n", clientFd);
    send(clientFd, msg, strlen(msg), 0);
    return new Client(clientFd);
}

void ClientsManager::addClient(const int &fd, Client *client) {
    this->addOne(fd, client);
}


void ClientsManager::removeClient(const int &fd) {
    struct kevent evSet;

    EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(this->_eventLoop.getEventLoop(), &evSet, 1, NULL, 0, NULL);
    this->removeOne(fd);
}