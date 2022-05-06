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
        std::map<int, const TCPServer *>  _client_server_map;
    public:
        ClientsManager(const EventLoop &eventLoop);
        ~ClientsManager(void);
        ClientsManager(const ClientsManager &instance);

        ClientsManager &operator=(const ClientsManager &rhs);

        Client                              *createNewClient(const int &fd);

        // wrappers over manager methods
        const std::map<int, Client *>       &getClients(void) const;
        const Client                        *getClient(const int &fd);
        const TCPServer                     *getServerByClientFd(const int &fd);
        void                                addClient(const TCPServer *server, Client *client);
        void                                removeClient(const int &fd);
};

std::ostream &operator<<(std::ostream &o, ClientsManager &instance);