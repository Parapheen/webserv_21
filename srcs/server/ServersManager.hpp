#pragma once
#include <sys/event.h>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "Manager.hpp"
#include "EventLoop.hpp"
#include "TCPServer.hpp"

class ServersManager : public Manager<TCPServer> {
    private:
        std::map<std::string, TCPServer *>    _servers; // name: server instance

    public:
        ServersManager(const EventLoop &eventLoop);
        ServersManager(const ServersManager &instance);

        ServersManager &operator=(const ServersManager &rhs);

        ~ServersManager(void);

        const std::map<std::string, TCPServer *> &getServers(void) const;

        // human readable wrappers over Manager methods
        void                                addSocket(const int &fd, TCPServer *server);
        const std::map<int, TCPServer *>    &getSockets(void) const;
        const TCPServer                     *getServerBySocket(const int &fd) const;
        void                                removeSocket(const int &fd);

        void                                addServer(const std::string &serverName, TCPServer *server);
        void                                init(const std::vector<ServerCfg> &servers);
};

std::ostream &operator<<(std::ostream &o, ServersManager &instance);