#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>

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

    
}