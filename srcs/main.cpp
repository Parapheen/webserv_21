#include "server/ServersManager.hpp"

int main(void)
{
    ServersManager webserv = ServersManager();

    webserv.run();
    return (0);
}