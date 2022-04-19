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
#include "Manager.hpp"

class ClientsManager : public Manager<Client> {
    public:
        ClientsManager(const EventLoop &eventLoop);
        ~ClientsManager(void);
        ClientsManager(const ClientsManager &instance);

        ClientsManager &operator=(const ClientsManager &rhs);

        Client                              *createNewClient(const int &fd);

        // wrappers over manager methods
        const std::map<int, Client *>       &getClients(void) const;
        const Client                        *getClient(const int &fd);
        void                                addClient(const int &fd, Client *client);
        void                                removeClient(const int &fd);
};

std::ostream &operator<<(std::ostream &o, ClientsManager &instance);