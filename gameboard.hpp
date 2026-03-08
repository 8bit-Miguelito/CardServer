#include<iostream>
#include<vector>

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#define MAX_PER_SUIT 13
#define MAX_CARDS 52

class Gameboard 
{
	private:
		struct Card
		{
			int rank;
			std::string suit;

			Card() { rank = 0; suit = ""; }
		};

		struct Player 
		{
			Card* hand;
			int fd;
			int numCards;

			Player(const int& fd) : hand(new Card[26]), fd(fd), numCards(0) {}
		};

		size_t numPlayers;
		Card* deck;
		const std::string suits[4] = {"HEARTS","DIAMONDS","CLUBS","SPADES"};
		std::vector<Player*> players;

		std::string rankToString(int r)
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

			return std::to_string(r);
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
		}

		void shuffle()
		{
			for(size_t i = 0; i < MAX_CARDS; i++)
			{
				int randNum = rand() % MAX_CARDS;
				Card temp = deck[randNum];
				deck[randNum] = deck[i];
				deck[i] = temp;
			}
		}

		void deal()
		{
			size_t currPlayer = 0;
			int idx = 0;
			for(size_t i = 0; i < MAX_CARDS; i++)
			{
				if((idx % numPlayers) == 0) idx = 0;
				Player* currentPlayer = players.at(idx);
				if(currPlayer >= numPlayers) currPlayer = 0;
				currentPlayer->hand[currentPlayer->numCards] = deck[i];
				currentPlayer->numCards++;
				idx++;
			}
		}

	public:
		Gameboard(int numPlayers) {
			this->numPlayers = numPlayers;
			deck = new Card[MAX_CARDS];
			setDeck();
		}

		void game(const std::set<int>& activePlayers)
		{
			for(auto player_fd : activePlayers)
			{
				players.push_back(new Player(player_fd));
			}
			std::cout << "players" << std::endl;

			shuffle();
			std::cout << "shuffle" << std::endl;
			deal();
			std::cout << "deal" << std::endl;
		}

		void viewDeck()
		{
			for (size_t i = 0; i < MAX_CARDS; i++)
			{
				std::string str_rank = rankToString(deck[i].rank);
				std::cout << str_rank << " of " << deck[i].suit << std::endl;
			}
			
		}

		void viewHand(int fd)
		{
			int idx = -1;
			for (size_t i = 0; i < numPlayers; i++)
			{
				try {
					if (players.at(i)->fd == fd) { idx = i; break; }
				} catch(std::out_of_range e)
				{
					std::cout << "Caught: " << e.what() << std::endl;
				}
			}

			Player *player = players.at(idx);
			for (int i = 0; i < player->numCards; i++)
			{
				std::cout << player->hand[i].rank << " of " << player->hand[i].rank << " ";
			}
			std::cout << std::endl;
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





















// class Player
// {
// 	private:
// 		struct Card {
			
// 			int rank;
// 			string suit;
		
// 			Card() {rank = 0; suit = "";}
// 			Card(int rank, string suit) : rank(rank) , suit(suit) {}
// 		};
		
// 		Card* hand;
// 		size_t numCards;

// 	public:
// 		Player()
// 		{
// 			hand = new Card[13];
// 			numCards = 0;
// 		}


// 		void displayCards()
// 		{
// 			for(size_t i = 0; i < numCards; i++)
// 			{
// 				cout << hand[i].rank << " of " << hand[i].suit << endl;
// 			}
// 		}
// };


#endif
		

