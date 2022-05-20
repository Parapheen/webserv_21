#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>
#include "Client.hpp"
#include "TCPServer.hpp"
#include "Manager.hpp"

class ClientsManager : public Manager<Client> {
    private:
        std::map<int, TCPServer *>  _client_server_map;
    public:
        ClientsManager(const EventLoop &eventLoop);
        ~ClientsManager(void);
        ClientsManager(const ClientsManager &instance);

        ClientsManager &operator=(const ClientsManager &rhs);

        Client                              *createNewClient(const int &fd);

        const std::map<int, Client *>       &getClients(void) const;
        Client                              *getClient(const int &fd);
        TCPServer                           *getServerByClientFd(const int &fd);
        const Response                      &getResponseByClientFd(const int &fd);
        const Request                       &getRequestByClientFd(const int &fd);
        void                                addClient(TCPServer *server, Client *client);
        void                                writeToClient(const int &fd);
        void                                removeClient(const int &fd);
        void                                setResponseToClient(const int &fd, const Response &response);
        void                                setRequestToClient(const int &fd, const Request &response);
};

std::ostream &operator<<(std::ostream &o, ClientsManager &instance);