#include "server/Webserver.hpp"

int main(void)
{
    Webserver webserv = Webserver();

    webserv.run();
    return (0);
}