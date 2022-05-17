#include "Webserver.hpp"

Webserver::Webserver(void) : _serversManager(new ServersManager(this->_eventLoop)),
    _clientsManager(new ClientsManager(this->_eventLoop)) { return ; }

Webserver::Webserver(const Webserver &instance) : _eventLoop(instance._eventLoop), _serversManager(instance._serversManager), _clientsManager(instance._clientsManager) {
    return ;
}

Webserver &Webserver::operator=(const Webserver &rhs) {
    if (this != &rhs) {
        this->_eventLoop = rhs._eventLoop;
        this->_serversManager = rhs._serversManager;
        this->_clientsManager = rhs._clientsManager;
    }
    return *this;
}

void    Webserver::handleEvent(const struct kevent &event) {
    // event socket equals to one of the server => new client
    if (TCPServer *server = this->_serversManager->getServerBySocket(event.ident)) {
        Client *newClient = this->_clientsManager->createNewClient(event.ident);
        this->_clientsManager->addClient(server, newClient);
    } // disconnected
    else if (event.flags & EV_EOF) {
        this->_clientsManager->removeClient(event.ident);
    } // read message from client
    else if (event.filter == EVFILT_READ || event.filter == EVFILT_WRITE) {
        // chunked reading
        this->_handleIO(event);
    }
    else
        this->_clientsManager->removeClient(event.ident);
}

void    Webserver::_handleIO(const struct kevent &event) {
    if (event.filter == EVFILT_READ)
        this->_handleRead(event);
    else if (event.filter == EVFILT_WRITE)
        this->_handleWrite(event);
}

void    Webserver::_handleRead(const struct kevent &event) {
    char buf[event.data];
    int recvRet;

    recvRet = recv(event.ident, buf, sizeof(buf), 0);
    if (recvRet == 0) {
        this->_clientsManager->removeClient(event.ident);
        return;
    }
    else if (recvRet == -1) {
        this->_clientsManager->removeClient(event.ident);
        return;
    }
    printf("client at socketfd %lu: %s", event.ident, buf);
    ServerCfg server = this->_clientsManager->getServerByClientFd(event.ident)->getServerConfig();
    Request request = Request();
    request.setConfig(server);
    Response response = request.parse(buf);
    this->_clientsManager->setResponseToClient(event.ident, response);
    this->_clientsManager->writeToClient(event.ident);
}

void    Webserver::_handleWrite(const struct kevent &event) {
    Response response = this->_clientsManager->getResponseByClientFd(event.ident);
    std::string responseStr = response.getResponse();

    std::cout << responseStr << std::endl;
    if (send(event.ident, responseStr.c_str(), strlen(responseStr.c_str()), 0) < 0)
        this->_clientsManager->removeClient(event.ident);
    this->_clientsManager->removeClient(event.ident);
}

void    Webserver::run(const std::vector<ServerCfg> &servers) {
    const int       evSize = this->_eventLoop.getEventListSize(); 
    struct kevent   eventList[evSize];

    memset(&eventList, 0, sizeof(eventList));
    this->_serversManager->init(servers);
    while (true) {
        int num_events = kevent(this->_eventLoop.getEventLoop(), NULL, 0, eventList, evSize, NULL);
        if (num_events == -1)
            throw std::system_error(EFAULT, std::generic_category());
        for (int i = 0; i < num_events; i++)
            this->handleEvent(eventList[i]);
    }
}

Webserver::~Webserver(void) { return ; }