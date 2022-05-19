#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "../http/Request.hpp"
#include "../http/Response.hpp"

// Interface for client connections
class Client {
    private:
        int                 _socketfd;
        Request             _currentRequest;
        Response            _currentResponse;

    public:
        Client(const int &fd);
        Client(Client const &instance);
        Client &operator=(const Client &rhs);

        ~Client(void);

        const int       &getSocketFd(void) const;
        const Response  &getResponse(void) const;

        void            setSocketFd(const int &fd);
        void            setResponse(const Response &response);
};

std::ostream &operator<<(std::ostream &o, Client &instance);