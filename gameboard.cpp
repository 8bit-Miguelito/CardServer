#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/event.h>
#include<sys/time.h>
#include<sys/types.h>

#include "gameboard.hpp"

const int maxConnections = 2;

int main(void)
{
    int numPlayers = 1;
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8075);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int client_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (client_fd == 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (bind(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(client_fd, 10) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    fd_set active, ready;
    FD_SET(client_fd, &active);
    int max_fd = client_fd;

    while(numPlayers != maxConnections)
    {
        ready = active;
        if (select(max_fd + 1, &ready, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i <= max_fd; i++)
        {
            if (FD_ISSET(i, &ready))
            {
                if(i == client_fd)
                {
                    std::cout << "New connection" << std::endl;
                    int accept_fd = accept(client_fd, 0, 0);
                    if (accept_fd > max_fd) max_fd = accept_fd;
                    FD_SET(accept_fd, &active);
                    numPlayers++;
                } else 
                {
                    char temp[1024];
                    ssize_t in = recv(i, temp, 1024, 0);    
                    if (in > 0)
                    {
                        char buf[] =  "Waiting on other players...\n";
                        ssize_t n = strnlen(buf, 64);
                        if (send(i, buf, n, 0) == - 1)
                        {
                            perror("send");
                            exit(EXIT_FAILURE);
                        }
                    } else
                    {
                        if (in == 0)
                        {
                            std::cout << "client " << i << " disconnected" << std::endl;
                            close(i);
                            FD_CLR(i, &active);
                        } else
                        {
                            perror("recv");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        }
    }

    std::cout << "Let's continue" << std::endl;
    return EXIT_SUCCESS;
}