#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <unistd.h>

class TCPServer {
    private:
        int _socketfd; // server socket for LISTEN
        sockaddr_in _sockaddr;

        int _port;
        int _numberOfConnections;

        void _createSocket(void);
        void _bind(void);
        void _listen(void);
        void _init(void);
        void _accept(void);

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

        // exceptions
        class SocketCreationException : public std::exception {
            virtual const char *what() const throw();
        };
        class SocketBindingException : public std::exception {
            virtual const char *what() const throw();
        };
        class SocketListenException : public std::exception {
            virtual const char *what() const throw();
        };
};

std::ostream &operator<<(std::ostream &o, TCPServer &instance);