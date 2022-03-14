#include "TCPServer.hpp"

TCPServer::TCPServer(void) { return; }

void TCPServer::_createSocket(void) {
    this->_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketfd == -1)
        throw std::system_error(EFAULT, std::generic_category());
}

void TCPServer::_bind(void) {
  this->_sockaddr.sin_family = AF_INET;
  this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
  this->_sockaddr.sin_port = htons(9999); // port
  if (bind(this->_socketfd, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
    throw std::system_error(EFAULT, std::generic_category());
}

void TCPServer::_listen(void) {
    if (listen(this->_socketfd, 10) < 0) // number of connections
        throw std::system_error(EFAULT, std::generic_category());
}

void TCPServer::_init(void) {
    this->_createSocket();
    this->_bind();
    this->_listen();
}

void TCPServer::_accept(void) {
    int clientSocket;
    sockaddr clientaddr;
    long valRead;

    while (true)
    {
        if ((clientSocket = accept(this->_socketfd, (struct sockaddr *)&clientaddr, (socklen_t*)&clientaddr)) < 0)
            throw std::system_error(EFAULT, std::generic_category());
        while (true) {
            char buffer[30000] = {0};
            valRead = recv(clientSocket, buffer, 30000, 0);
            if (valRead == 0)
                break ;
            send(clientSocket, buffer, valRead, 0);
        }
        close(clientSocket);
    }
}

void TCPServer::start(void) {
    try
    {
        this->_init();
        this->_accept();
    }
    catch(const std::exception& e)
    {
        std::cerr << "FATAL: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

TCPServer::~TCPServer(void) {
    // cleanup, perhaps closing connections
    close(this->_socketfd);
    return;
}