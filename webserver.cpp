#include "iostream"
//#include <sys/socket.h>

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Wrong numbers of arguments\n";
        return 1;
    }
    std::cout << argv[1] << std::endl;
    struct sockaddr_in serv_addr, client_addr;

    int sockfd, portno;
    memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cout << "sock fd is failure\n";
        return 1;
    }
    //fcntl(sockfd, F_SETFL, O_NONBLOCK);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = std::atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    //serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(portno);

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cout << "setsockop failed\n";
        return 1;
    }


    int id = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (id < 0)
    {
        std::cout << "bind failed\n";
        std::cout << strerror(errno) << std::endl;
        return 1;
    }
    

    // int con = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    // if (con < 0)
    // {
    //     std::cout << "connect failed\n";
    //     std::cout << strerror(errno) << std::endl;
    //     return 1;
    // }

    int res = listen(sockfd, SOMAXCONN);
    if (res < 0)
    {
        std::cout << "listen failed\n";
        return 1;
    }

    // while (1)
    // {
        int clientlen = sizeof(client_addr);
        int client_socket = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *) &clientlen);
        std::cout << "client socket: " << client_socket << std::endl;
        if (client_socket < 0)
        {
            std::cout << "client socket failed\n";
            std::cout << strerror(errno) << std::endl;
            return 1;
        }
    //}
    

    printf("server: got connection from %s port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


    // This send() function sends the 13 bytes of the string to the new socket
    send(client_socket, "Hello, world 21!\n", 16, 0);
    send(client_socket, "Send hello to server!\n", 20, 0);

    char buffer[1024];
    bzero(buffer, 1024);
    int bytesRec  = recv(client_socket, buffer, 1024, 0);
    if (bytesRec < 0)
        std::cout << "ERROR reading from socket\n";
    std::cout << buffer << std::endl;

    return 0;
}