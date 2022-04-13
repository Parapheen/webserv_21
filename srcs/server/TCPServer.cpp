#include "TCPServer.hpp"

TCPServer::TCPServer(const int &eventLoop, const int &port) : _eventLoop(eventLoop), _port(port) { 
    try
    {
        this->_init();
    }
    catch(const std::exception& e)
    {
        std::cerr << "FATAL: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void TCPServer::_createSocket(void) {
    this->_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketfd == -1)
        throw std::system_error(EFAULT, std::generic_category());
    fcntl(this->_socketfd, F_SETFL, O_NONBLOCK);
}

void TCPServer::_bind(void) {
    this->_sockaddr.sin_family = AF_INET;
    this->_sockaddr.sin_addr.s_addr = INADDR_ANY;
    this->_sockaddr.sin_port = htons(this->_port); // port
    if (bind(this->_socketfd, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
        throw std::system_error(EFAULT, std::generic_category());
}

void TCPServer::_listen(void) {
    if (listen(this->_socketfd, 10) < 0) // number of connections
        throw std::system_error(EFAULT, std::generic_category());
    std::cout << "Listening... " << this->_socketfd << " " << this->_port << std::endl;
}

void TCPServer::_bindToEventLoop(void) {
    struct kevent evSet;

    EV_SET(&evSet, this->_socketfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(this->_eventLoop, &evSet, 1, NULL, 0, NULL);
    this->_clientManager.setEventLoop(this->_eventLoop);
}

void TCPServer::_init(void) {
    this->_createSocket();
    this->_bindToEventLoop();
    this->_bind();
    this->_listen();
}


void TCPServer::handle(const struct kevent &event) {
    if ((int)event.ident == this->_socketfd) {
        try {
            this->_clientManager.addClient(event.ident);
        }
        catch(const std::exception& e) {
            throw e;
        }
    } // client disconnected
    else if (event.flags & EV_EOF) {
        try {
            this->_clientManager.deleteClient(event.ident);
        }
        catch(const std::exception& e) {
            throw e;
        }
    } // read message from client
    else if (event.filter == EVFILT_READ) {
        // process http request
        char buf[2048];
        int bytes_read = recv(event.ident, buf, sizeof(buf) - 1, 0);
        buf[bytes_read] = 0;
        printf("client #%d: %s", this->_clientManager.getClient(event.ident), buf);
        fflush(stdout);
    }
}

// void TCPServer::start(void) {
//     try
//     {
//         this->_init();
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << "FATAL: " << e.what() << std::endl;
//         exit(EXIT_FAILURE);
//     }
// }

const int   &TCPServer::getSocketFd(void) const {
    return this->_socketfd;
}

TCPServer::~TCPServer(void) {
    // cleanup, perhaps closing connections
    close(this->_socketfd);
}