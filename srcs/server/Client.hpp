#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

// Interface for client connections
class Client {
    private:
        int                 _socketfd;
        // sockaddr_in         _sockaddr;

    public:
        Client(const int &fd);
        Client(Client const &instance);
        Client &operator=(const Client &rhs);

        ~Client(void);

        const int   &getSocketFd(void) const;
        void        setSocketFd(const int &fd);
};

std::ostream &operator<<(std::ostream &o, Client &instance);