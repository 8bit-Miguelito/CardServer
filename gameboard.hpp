#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdio.h>
#include<vector>

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

const int MAX_PER_SUIT = 13;
const int MAX_CARDS = 52;

using namespace std;

class Server 
{
	private:
		int maxConnections;
		int client_fd;
		int max_fd;

		set<int> connections;
		struct sockaddr_in serv_addr;
		fd_set active, ready; //Active stores all available connection, ready will be modified by select function
	public:
		Server(int maxConnections)
		{
			this->maxConnections = maxConnections;
			serv_addr.sin_family = AF_INET; //Set protocol to IP
    		serv_addr.sin_port = htons(8075); //Assign port number for connection (convert bytes to network byte order)
    		serv_addr.sin_addr.s_addr = INADDR_ANY; //Allow incoming connections from any IP address

    		client_fd = socket(PF_INET, SOCK_STREAM, 0); //Open socket endpoint that allows IP connections and sends/receives streams of bytes
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

    		if (listen(client_fd, maxConnections) == -1) //Allow socket to listen for incoming connections
    		{
        		perror("listen");
        		exit(EXIT_FAILURE);
    		}

			max_fd = client_fd;
		}

		set<int> getConnections() { return connections; }

		//Gather all players who are ready to play
		//Return false if there is an issue connecting to a player
		//Return true if all connections are succesful
		bool GatherConnections()
		{
			connections.insert(STDIN_FILENO);
			while(connections.size() != maxConnections) 
    		{
        		ready = active;
        		if (select(max_fd + 1, &ready, NULL, NULL, NULL) < 0) //Select linearly searches through the ready set for any connections or bytes to be read
        		{
        			perror("select");
            		return false;
        		}

        		for(int i = 0; i <= max_fd; i++)
        		{
            		if (FD_ISSET(i, &ready)) //If the fd is in the ready set:
            		{
                		if(i == client_fd) //New incoming connection
                		{
                    		cout << "New connection" << endl;
                    		int accept_fd = accept(client_fd, 0, 0); //Accept the connection
                    		if (accept_fd > max_fd) max_fd = accept_fd; //Update max fd in set
                    		FD_SET(accept_fd, &active); //Add new fd to the active set
                    		connections.emplace(accept_fd); 
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
                    		        cout << "client " << i << " disconnected" << endl;
                    		        close(i);
                    		        FD_CLR(i, &active); //Remove client from the active set 
                    		        connections.erase(i);
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

			return true;
		}

};

class Gameboard 
{
	private: 
		void setFDs()
		{
			set<int> connections = server->getConnections();
			int i = 0;
			for(auto connection : connections)
			{
				players.at(i)->fd = connection;
			}
		}
	protected:
		struct Card
		{
			int rank;
			string suit;

			Card() { rank = 0; suit = ""; }
			bool operator<(const Card& other) const
			{	
				if(this->rank != other.rank)
				{
					return this->rank < other.rank;
				}

				return this->suit < other.suit;
			}
		};

		struct Player 
		{
			set<Card> hand; //Tree for more efficient search
			int fd;
			int numCards;

			Player(const int& fd) : fd(fd), numCards(0) {}
		};

		const string suits[4] = {"HEARTS","DIAMONDS","CLUBS","SPADES"};

		string rankToString(int r)
		{
			r += 2;
			switch(r)
			{
				case 11:
					return "JACK";
				case 12:
					return "QUEEN";
				case 13:
					return "KING";
				case 14:
					return "ACE";
			}

			return to_string(r);
		}

		void shuffle()
		{
			srand(time(0));
			for(size_t i = 0; i < MAX_CARDS; i++)
			{
				int randNum = rand() % MAX_CARDS;
				Card temp = deck[randNum];
				deck[randNum] = deck[i];
				deck[i] = temp;
			}
		}

		void setDeck()
		{
			size_t suitLimit = MAX_PER_SUIT;
			size_t currSuit = 0;
			for(size_t i = 0 ; i < MAX_CARDS; i++)
			{
				if (suitLimit == i)
				{
					currSuit++;
					suitLimit += MAX_PER_SUIT;
				}

				deck[i].rank = i % MAX_PER_SUIT;
				deck[i].suit = suits[currSuit];
			}

			shuffle();
		}
		//abstract methods
		virtual void deal() = 0;
		virtual void play(const set<int>& activePlayers) = 0;

		inline void addToHand(Player*& player, const Card& card)
		{
			player->numCards++;
			player->hand.insert(card);
		}
		
		//variables 
		Server* server;
		size_t numPlayers;
		Card* deck;
		vector<Player*> players;

	public:
		Gameboard(const set<int>& players) {
			this->numPlayers = players.size();
			server = new Server(players.size());
			for (auto player : players)
			{
				this->players.push_back(new Player(player)); 
			}
			deck = new Card[MAX_CARDS];
			setDeck();
		}


		//For testing purposes
		void viewDeck()
		{
			for (size_t i = 0; i < MAX_CARDS; i++)
			{
				std::string str_rank = rankToString(deck[i].rank);
				std::cout << i << ": " << str_rank << " of " << deck[i].suit << std::endl;
			}
			
		}

		void viewHand(int fd)
		{
			Player* player;
			for (size_t i = 0; i < numPlayers; i++)
			{
				if (players.at(i)->fd == fd) player = players.at(i);
			}

			std::cout << player->numCards << std::endl;

			std::set<Card>* temp = &(player->hand);
			for (auto card : *temp)
			{
				std::cout << card.rank << " of " << card.suit << std::endl;
			}
		}

		~Gameboard()
		{
			for(size_t i = 0; i < numPlayers; i++)
			{
				delete players.at(i);
			}

			delete[] deck;
		}
};

class Blackjack : public Gameboard
{
	private:
		size_t dealt;

		void deal() override
		{
			for(size_t i = 0; i < numPlayers; i++)
			{
				players.at(i)->hand.insert(deck[dealt]);
				dealt++;
			}
		}

		void checkInputs(const set<int>& activePlayers)
		{
			
		}
	public:
		Blackjack(const set<int>& players) : Gameboard(players) {}

		void play(const set<int>& activePlayers) override 
		{ 
			dealt = 0;
			size_t currPlayer = 0;
			while(1)
			{
				if ((currPlayer % numPlayers) == 0) currPlayer = 0;

				if (dealt == 0)
				{
					deal();
				} else
				{
					checkInputs(activePlayers);
				}
			}
		}
};

#endif