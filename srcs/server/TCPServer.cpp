#include "TCPServer.hpp"

TCPServer::TCPServer(void) { return; }

void TCPServer::_createSocket(void) {
    this->_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketfd == -1)
        throw TCPServer::SocketCreationException();
}

void TCPServer::_bind(void) {
  this->_sockaddr.sin_family = AF_INET;
  this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
  this->_sockaddr.sin_port = htons(9999); // port
  if (bind(this->_socketfd, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
    throw TCPServer::SocketBindingException();
}

void TCPServer::_listen(void) {
    if (listen(this->_socketfd, 10) < 0) // number of connections
        throw TCPServer::SocketListenException();
}

void TCPServer::_init(void) {
    this->_createSocket();
    this->_bind();
    this->_listen();
}

void TCPServer::_accept(void) {
    int clientSocket;
    long valRead;

    std::string hello = "Hello world\n";

    while (1)
    {
        if ((clientSocket = accept(this->_socketfd, (struct sockaddr *)&this->_sockaddr, (socklen_t*)&this->_sockaddr))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valRead = read(clientSocket, buffer, 30000);
        write(clientSocket, hello.c_str(), strlen(hello.c_str()));
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
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

const char *TCPServer::SocketCreationException::what() const throw() { return ("Failed to create socket"); }
const char *TCPServer::SocketBindingException::what() const throw() { return ("Failed to bind to socket"); }
const char *TCPServer::SocketListenException::what() const throw() { return ("Failed to listen to socket"); }

TCPServer::~TCPServer(void) {
    // cleanup, perhaps closing connections
    close(this->_socketfd);
    return;
}