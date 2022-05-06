#include "Webserver.hpp"

Webserver::Webserver(void) : _serversManager(new ServersManager(this->_eventLoop)),
    _clientsManager(new ClientsManager(this->_eventLoop)) { return ; }

Webserver::Webserver(const Webserver &instance) {
    *this = instance;
    return ;
}

Webserver &Webserver::operator=(const Webserver &rhs) {
    if (this->_eventLoop.getEventLoop() != rhs._eventLoop.getEventLoop())
        this->_eventLoop = rhs._eventLoop;
    return *this;
}

void    Webserver::handleEvent(const struct kevent &event) {
    // event socket equals to one of the server => new client
    if (const TCPServer *server = this->_serversManager->getServerBySocket(event.ident)) {
        try {
            Client *newClient = this->_clientsManager->createNewClient(event.ident);
            this->_clientsManager->addClient(server, newClient);
        }
        catch(const std::exception& e) {
            throw e;
        }
    } // disconnected
    else if (event.flags & EV_EOF) {
        try {
            this->_clientsManager->removeClient(event.ident);
        }
        catch(const std::exception& e) {
            throw e;
        }
    } // read message from client
    else if (event.filter == EVFILT_READ) {
        // process http request
        // if msg more than buffer
        // chunked reading
        char buf[2048];
        int bytes_read = recv(event.ident, buf, sizeof(buf) - 1, 0);
        buf[bytes_read] = 0;
        printf("client at socketfd %lu: %s", event.ident, buf);
        ServerCfg server = this->_clientsManager->getServerByClientFd(event.ident)->getServerConfig();
        Request request = Request();
        request.setConfig(server); // pass to constructor instead
        Response response = request.parse(buf);
        std::string responseStr = response.getResponse();
        std::cout << responseStr << std::endl;
        send(event.ident, responseStr.c_str(), strlen(responseStr.c_str()), 0);
        this->_clientsManager->removeClient(event.ident);
    }
}

void    Webserver::run(const std::vector<ServerCfg> &servers) {
    const int       evSize = this->_eventLoop.getEventListSize(); 
    struct kevent   eventList[evSize];

    this->_serversManager->init(servers);
    while (true) {
        int num_events = kevent(this->_eventLoop.getEventLoop(), NULL, 0, eventList, evSize, NULL);
        for (int i = 0; i < num_events; i++) {
            int eventSocket = static_cast<int>(eventList[i].ident);
            std::cout << "Event socket: " << eventSocket << std::endl;
            this->handleEvent(eventList[i]);
        }
    }
}

Webserver::~Webserver(void) {
    delete this->_serversManager;
    delete this->_clientsManager;
    return ;
}