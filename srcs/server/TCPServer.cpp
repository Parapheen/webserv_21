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
    this->_sockaddr.sin_port = htons(8004); // port
    if (bind(this->_socketfd, (struct sockaddr*)&this->_sockaddr, sizeof(sockaddr)) < 0)
    throw std::system_error(EFAULT, std::generic_category());
}

void TCPServer::_listen(void) {
    if (listen(this->_socketfd, 10) < 0) // number of connections
        throw std::system_error(EFAULT, std::generic_category());
}

void TCPServer::_createEventLoop(void) {
    this->_eventLoop = kqueue();
    struct kevent evSet;

    EV_SET(&evSet, this->_socketfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(this->_eventLoop, &evSet, 1, NULL, 0, NULL);
    this->_clientManager.setEventLoop(this->_eventLoop);
}

void TCPServer::_init(void) {
    this->_createSocket();
    this->_createEventLoop();
    this->_bind();
    this->_listen();
}


void TCPServer::_run(void) {
    struct kevent evList[10];

    while (true) {
        int num_events = kevent(this->_eventLoop, NULL, 0, evList, 10, NULL);
        for (int i = 0; i < num_events; i++) {
            if ((int)evList[i].ident == this->_socketfd) {
                // add client logic
                try {
                    this->_clientManager.addClient(evList[i].ident);
                }
                catch(const std::exception& e) {
                    throw e;
                }
            } // client disconnected
            else if (evList[i].flags & EV_EOF) {
                try {
                    this->_clientManager.deleteClient(evList[i].ident);
                }
                catch(const std::exception& e) {
                    throw e;
                }
            } // read message from client
            else if (evList[i].filter == EVFILT_READ) {
                char buf[2048];
                int bytes_read = recv(evList[i].ident, buf, sizeof(buf) - 1, 0);
                buf[bytes_read] = 0;
                printf("client #%d: %s", this->_clientManager.getClient(evList[i].ident), buf);
                fflush(stdout);
            }
        }
    }
}

void TCPServer::start(void) {
    try
    {
        this->_init();
        this->_run();
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
}