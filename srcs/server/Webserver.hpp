#pragma once
#include "Manager.hpp"
#include "ServersManager.hpp"
#include "ClientsManager.hpp"
#include "EventLoop.hpp"

class Webserver {
    private:
        EventLoop       _eventLoop;
        ServersManager  *_serversManager;
        ClientsManager  *_clientsManager;

    public:
        Webserver(void);

        Webserver(const Webserver &instance);
        Webserver &operator=(const Webserver &rhs);

        ~Webserver(void);

        void        handleEvent(const struct kevent &event);
        void        run(const std::vector<ServerCfg> &servers);
};