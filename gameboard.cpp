#include<iostream>
#include<set>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/event.h>
#include<sys/time.h>
#include<sys/types.h>

#include "gameboard.hpp"

int SetServer(struct sockaddr_in& serv_addr)
{
    serv_addr.sin_family = AF_INET; //Set protocol to IP
    serv_addr.sin_port = htons(8075); //Assign port number for connection (convert bytes to network byte order)
    serv_addr.sin_addr.s_addr = INADDR_ANY; //Allow incoming connections from any IP address

    int client_fd = socket(PF_INET, SOCK_STREAM, 0); //Open socket endpoint that allows IP connections and sends/receives streams of bytes
    if (client_fd == 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    if (::bind(client_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) //Bind a name to a socket
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(client_fd, 10) == -1) //Allow socket to listen for incoming connections
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return client_fd;
}

const int maxConnections = 2; //Temporarily 2 for testing purposes

int main(void)
{
    //Tree to hold current players file descriptors
    std::set<int> activePlayers;
    //Add the first player aka the host
    activePlayers.emplace(STDIN_FILENO);

    //struct for socket address and info
    struct sockaddr_in serv_addr;
    int client_fd = SetServer(serv_addr);
    
    //Create two fd_sets for event-driven concurrency
    fd_set active, ready; //Active stores all available connection, ready will modified by select function
    FD_SET(STDIN_FILENO, &active);//Add STDIN_FILENO since host will also be a player 
    FD_SET(client_fd, &active); //Add client_fd to check for incoming connections
    int max_fd = client_fd; 

    while(activePlayers.size() != maxConnections) 
    {
        ready = active;
        if (select(max_fd + 1, &ready, NULL, NULL, NULL) < 0) //Select linearly searches through the ready set for any connections or bytes to be read
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i <= max_fd; i++)
        {
            if (FD_ISSET(i, &ready)) //If the fd is in the ready set:
            {
                if(i == client_fd) //New incoming connection
                {
                    std::cout << "New connection" << std::endl;
                    int accept_fd = accept(client_fd, 0, 0); //Accept the connection
                    if (accept_fd > max_fd) max_fd = accept_fd; //Update max fd in set
                    FD_SET(accept_fd, &active); //Add new fd to the active set
                    activePlayers.emplace(accept_fd); 
                } else 
                {
                    char temp[1024];
                    ssize_t in = recv(i, temp, 1024, 0); //Check if client has attempted to send anything
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
                        if (in == 0) //EOF sent if client has disconnected
                        {
                            std::cout << "client " << i << " disconnected" << std::endl;
                            close(i);
                            FD_CLR(i, &active); //Remove client from the active set 
                            activePlayers.erase(i);
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

    Blackjack game(activePlayers);
    game.deal();
    for (auto player : activePlayers)
    {
        game.viewHand(player);
    }



    return EXIT_SUCCESS;
}