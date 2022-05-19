#pragma once
#include "Manager.hpp"
#include "ServersManager.hpp"
#include "ClientsManager.hpp"
#include "../http/Request.hpp"
#include "EventLoop.hpp"

class Webserver {
    private:
        EventLoop       _eventLoop;
        ServersManager  *_serversManager;
        ClientsManager  *_clientsManager;

        void    _handleIO(const struct kevent &event);
        void    _handleRead(const struct kevent &event);
        void    _handleWrite(const struct kevent &event);

    public:
        Webserver(void);

        Webserver(const Webserver &instance);
        Webserver &operator=(const Webserver &rhs);

        ~Webserver(void);

        void        handleEvent(const struct kevent &event);
        void        run(const std::vector<ServerCfg> &servers);

        class IOException : public std::exception {
            virtual const char *what() const throw ();
        };
        class PeerConnectionClosedException : public std::exception {
            virtual const char *what() const throw ();
        };
};