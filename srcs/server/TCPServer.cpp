#include "TCPServer.hpp"

TCPServer::TCPServer(const TCPServer &instance) : _socketfd(instance.getSocketFd()), _sockaddr(instance._sockaddr),
    _eventLoop(instance._eventLoop), _config(instance._config), _numberOfConnections(instance._numberOfConnections) {
    return;
}

TCPServer::TCPServer(const int &eventLoop, const ServerCfg &config) : _eventLoop(eventLoop), _config(config) {
    try {
        this->_init();
    }
    catch(const std::exception& e) {
        std::cerr << "FATAL: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

TCPServer &TCPServer::operator=(const TCPServer &rhs) {
    if (this != &rhs) {
        this->_socketfd = rhs._socketfd;
        this->_sockaddr = rhs._sockaddr;
        this->_eventLoop = rhs._eventLoop;
        this->_config = rhs._config;
        this->_numberOfConnections = rhs._numberOfConnections;
    }
    return *this;
}

void TCPServer::_createSocket(void) {
    this->_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketfd == -1)
        throw std::system_error(EFAULT, std::generic_category());
    fcntl(this->_socketfd, F_SETFL, O_NONBLOCK);
}

void TCPServer::_bind(void) {
    this->_sockaddr.sin_family = AF_INET;
    this->_sockaddr.sin_addr.s_addr = INADDR_ANY; // host
    this->_sockaddr.sin_port = htons(this->_config.getPort()); // port
    if (bind(this->_socketfd, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
        throw std::system_error(EFAULT, std::generic_category());
}

void TCPServer::_listen(void) {
    if (listen(this->_socketfd, 100) < 0) // number of connections
        throw std::system_error(EFAULT, std::generic_category());
    std::cout << "Listening... " << this->_socketfd << " " << this->_config.getPort() << std::endl;
}

void TCPServer::_bindToEventLoop(void) {
    struct kevent evSet;

    EV_SET(&evSet, this->_socketfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(this->_eventLoop, &evSet, 1, NULL, 0, NULL);
}

void TCPServer::_init(void) {
    this->_createSocket();
    this->_bindToEventLoop();
    this->_bind();
    this->_listen();
}

const int   &TCPServer::getSocketFd(void) const { return this->_socketfd; }
const ServerCfg &TCPServer::getServerConfig(void) const { return this->_config; }

TCPServer::~TCPServer(void) {
    close(this->_socketfd);
}