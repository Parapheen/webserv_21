#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <iostream>
// #include <algorithm>
// #include <vector>
#include <string>
#include <unistd.h>
#include "ClientsManager.hpp"

class TCPServer {
    private:
        int                 _socketfd; // server socket for LISTEN
        sockaddr_in         _sockaddr;

        int                 _eventLoop; // kevent loop

        int                 _port; // port is parsed by config
        int                 _numberOfConnections; // not sure here

        ClientsManager      _clientManager; // manage client connections

        // std::vector<int>    _clients;

        void                _createSocket(void);
        void                _createEventLoop(void);
        void                _bind(void);
        void                _listen(void);
        void                _init(void);
        // int                 _getClient(const int &fd) const;
        // void                _addClient(const uintptr_t &ident);
        // void                _deleteClient(const uintptr_t &ident);
        void                _run(void);

    public:
        TCPServer(void);
        ~TCPServer(void);
        TCPServer(const TCPServer &instance);

        TCPServer &operator=(const TCPServer &rhs);

        // getters
        void getSocketFd(void) const;
        void getSocketAddr(void) const;

        // methods
        void start(void);
};

std::ostream &operator<<(std::ostream &o, TCPServer &instance);