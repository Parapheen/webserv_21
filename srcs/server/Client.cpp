#include "Client.hpp"

Client::Client(const int &fd) : _socketfd(fd) { return; }

Client::~Client(void) { return; }

Client::Client(const Client &instance) {
    *this = instance;
    return ;
}

Client &Client::operator=(const Client &rhs) {
    if (this->getSocketFd() != rhs.getSocketFd())
        this->_socketfd = rhs._socketfd;
    return *this;
}

const int &Client::getSocketFd(void) const {
    return this->_socketfd;
}

void    Client::setSocketFd(const int &fd) {
    this->_socketfd = fd;
    return ;
}
