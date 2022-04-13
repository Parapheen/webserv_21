#pragma once
#include <sys/event.h>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "TCPServer.hpp"

class ServersManager {
    private:
        int                                 _eventLoop; // kevent loop
        std::map<std::string, TCPServer>    _servers; // name: server instance
        std::map<int, TCPServer>            _sockets; // socket: server instance

    public:
        ServersManager(void);
        ServersManager(const ServersManager &instance);

        ServersManager &operator=(const ServersManager &rhs);

        ~ServersManager(void);

        const std::map<std::string, TCPServer> &getServers(void) const;
        const int       &getEventLoop(void) const;
        void            run(void);
};