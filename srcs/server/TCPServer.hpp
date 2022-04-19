#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "ClientsManager.hpp"

class TCPServer {
    private:
        int                 _socketfd; // server socket for LISTEN
        sockaddr_in         _sockaddr;

        int                 _eventLoop; // kevent loop !ptr!
        
        int                 _port; // port is parsed by config
        int                 _numberOfConnections; // not sure here

        void                _createSocket(void);
        void                _bindToEventLoop(void);
        void                _bind(void);
        void                _listen(void);
        void                _init(void);

    public:
        TCPServer(const int &eventLoop, const int &port); // more complex constructor needed
        ~TCPServer(void);
        TCPServer(const TCPServer &instance);

        TCPServer &operator=(const TCPServer &rhs);

        // getters
        const int       &getSocketFd(void) const;
        const sockaddr  &getSocketAddr(void) const;
};

std::ostream &operator<<(std::ostream &o, TCPServer &instance);