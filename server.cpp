#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/event.h>
#include<sys/time.h>



#define MAX_CONNECT 10


int main(void)
{
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8075);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int client_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (client_fd == 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    if (bind(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(client_fd, 10 == -1))
    {
        perror("listen");
        return EXIT_FAILURE;
    }

    fd_set active, ready;
    FD_ZERO(&active);
    FD_SET(STDIN_FILENO, &active);
    FD_SET(client_fd, &active);

    int max_fd = client_fd;
    std::cout << "about to enter loop" << std::endl;
    while(1)
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
                std::cout << "fd " << i << " ready" << std::endl;
                if (i == client_fd)
                {
                    int new_fd = accept(client_fd, 0, 0);
                    if (new_fd > max_fd) max_fd = new_fd;
                    FD_SET(new_fd, &active);
                    std::cout << "new connection added\n";
                }
                else
                {
                    char buf[20];
                    ssize_t n = read(i, buf, 20);
                    if (n > 0)
                    {
                        write(STDOUT_FILENO, buf, n);
                    } else
                    {
                        close(i);
                        FD_CLR(i, &active);
                    }
                }
            }   
        }
    }
}