#include "server/TCPServer.hpp"

int main(void)
{
    TCPServer server = TCPServer();

    server.start();
    return (0);
}