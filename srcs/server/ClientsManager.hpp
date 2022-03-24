#pragma once
#include <algorithm>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <iostream>

class ClientsManager {
    private:
        std::vector<int>    _clients;
        int                 _eventLoop;

    public:
        ClientsManager(void);
        ~ClientsManager(void);
        ClientsManager(const ClientsManager &instance);

        ClientsManager &operator=(const ClientsManager &rhs);

        const std::vector<int>    &getClients(void) const;
        int                 getClient(const int &fd) const;
        void                addClient(const uintptr_t &ident);
        void                deleteClient(const uintptr_t &ident);

        void                setEventLoop(const int &eventLoop);
};