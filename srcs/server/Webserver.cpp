#include "Webserver.hpp"

Webserver::Webserver(void) {
    this->_serversManager = new ServersManager(this->_eventLoop);
    this->_clientsManager = new ClientsManager(this->_eventLoop);
    return ;
}

Webserver::Webserver(const Webserver &instance) {
    *this = instance;
    return ;
}

Webserver &Webserver::operator=(const Webserver &rhs) {
    // TODO: finish here
    (void) rhs;
    return *this;
}

void    Webserver::handleEvent(const struct kevent &event) {
    // event socket equals to one of the server => new client
    if (this->_serversManager->getServerBySocket(event.ident)) {
        try {
            Client *newClient = this->_clientsManager->createNewClient(event.ident);
            this->_clientsManager->addClient(event.ident, newClient);
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
        char buf[2048];
        int bytes_read = recv(event.ident, buf, sizeof(buf) - 1, 0);
        buf[bytes_read] = 0;
        printf("client at socketfd %lu: %s", event.ident, buf);
        // this->_clientsManager->removeClient(event.ident);
    }
}

void    Webserver::run(void) {
    // init servers
    TCPServer   *serverOne = new TCPServer(this->_eventLoop.getEventLoop(), 8088);
    TCPServer   *serverTwo = new TCPServer(this->_eventLoop.getEventLoop(), 8089);
    const int   evSize = this->_eventLoop.getEventListSize(); 
    struct kevent eventList[evSize];

    this->_serversManager->addServer("serverOne", serverOne);
    this->_serversManager->addServer("serverTwo", serverTwo);
    while (true) {
        int num_events = kevent(this->_eventLoop.getEventLoop(), NULL, 0, eventList, evSize, NULL);
        for (int i = 0; i < num_events; i++) {
            int eventSocket = (int)eventList[i].ident;
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