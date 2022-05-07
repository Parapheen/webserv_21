#include "Client.hpp"

Client::Client(const int &fd) : _socketfd(fd) { return; }

Client::~Client(void) { return; }

Client::Client(const Client &instance) {
    this->_socketfd = instance._socketfd;
    this->_currentRequest = instance._currentRequest;
    this->_currentResponse = instance._currentResponse;
    return ;
}

Client &Client::operator=(const Client &rhs) {
    if (this->getSocketFd() != rhs.getSocketFd())
        this->_socketfd = rhs._socketfd;
    return *this;
}

const int &Client::getSocketFd(void) const { return this->_socketfd; }
const Response &Client::getResponse(void) const { return this->_currentResponse; }

void    Client::setSocketFd(const int &fd) { this->_socketfd = fd; }
void    Client::setResponse(const Response &response) { this->_currentResponse = response; }
