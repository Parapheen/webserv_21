#include "ServersManager.hpp"

ServersManager::ServersManager(void) {
    this->_eventLoop = kqueue();
    return;
}

const int &ServersManager::getEventLoop(void) const {
    return this->_eventLoop;
}

const std::map<std::string, TCPServer> &ServersManager::getServers(void) const {
    return this->_servers;
}

void ServersManager::run(void) {
    TCPServer   serverOne = TCPServer(this->getEventLoop(), 8004);
    TCPServer   serverTwo = TCPServer(this->getEventLoop(), 8005);
    struct kevent evList[10];

    this->_servers.insert(std::pair<std::string, TCPServer>("serverOne", serverOne));
    this->_servers.insert(std::pair<std::string, TCPServer>("serverTwo", serverTwo));
    int fdOne = serverOne.getSocketFd();
    int fdTwo = serverTwo.getSocketFd();
    this->_sockets.insert(std::pair<int, TCPServer>(fdOne, serverOne));
    this->_sockets.insert(std::pair<int, TCPServer>(fdTwo, serverTwo));
    while (true) {
        int num_events = kevent(this->_eventLoop, NULL, 0, evList, 10, NULL);
        for (int i = 0; i < num_events; i++) {
            int serverSocket = (int)evList[i].ident;
            this->_sockets.at(serverSocket).handle(evList[i]);
        }
    }
}

ServersManager::~ServersManager(void) { return; }