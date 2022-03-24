#include "ClientsManager.hpp"

ClientsManager::ClientsManager(void) { return; }
ClientsManager::~ClientsManager(void) { return; }

ClientsManager::ClientsManager(const ClientsManager &instance) {
    this->_clients = instance.getClients();
}

const std::vector<int> &ClientsManager::getClients(void) const { return this->_clients; }

int ClientsManager::getClient(const int &fd) const {
    std::vector<const int>::iterator iter = std::find(this->_clients.begin(), this->_clients.end(), fd);
    int index = std::distance(this->_clients.begin(), iter);
    return (index);
}

void ClientsManager::addClient(const uintptr_t &ident) {
    struct sockaddr_storage addr;
    struct kevent evSet;
    socklen_t socklen = sizeof(addr);
    
    int fd = accept(ident, (struct sockaddr *) &addr, &socklen);
    if (fd < 0)
        throw std::system_error(EFAULT, std::generic_category());
    this->_clients.push_back(fd);
    EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(this->_eventLoop, &evSet, 1, NULL, 0, NULL);
    char msg[80];
    sprintf(msg, "welcome! you are client #%d!\n", this->getClient(fd));
    send(fd, msg, strlen(msg), 0);
}


void ClientsManager::deleteClient(const uintptr_t &ident) {
    struct kevent evSet;

    EV_SET(&evSet, ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(this->_eventLoop, &evSet, 1, NULL, 0, NULL);
    std::vector<const int>::iterator iter = std::find(this->_clients.begin(), this->_clients.end(), ident);
    this->_clients.erase(iter);
}

void ClientsManager::setEventLoop(const int &eventLoop) {
    this->_eventLoop = eventLoop;
}