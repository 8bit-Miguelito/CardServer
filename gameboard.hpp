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
			bool operator<(const Card& other) const
			{	
				if(this->rank < other.rank)
				{
					if (this->suit == "SPADES" && other.suit != "SPADES")
					{
						return false;
					} else 
					{
						return true;
					}
				}

				return false;
			}
		};

		struct Player 
		{
			std::set<Card> hand; //Tree for more efficient search
			int fd;
			int numCards;

			Player(const int& fd) : fd(fd), numCards(0) {}
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

		void deal()
		{
			int idx = 0; //Index for current player tracking
			for(size_t i = 0; i < MAX_CARDS; i++)
			{
				if((idx % numPlayers) == 0) idx = 0; //Reset if we've reset max # of players
				Player* currentPlayer = players.at(idx); 

				auto ret = currentPlayer->hand.emplace(deck[i]);
				if (ret.second == false) { perror("emplace"); exit(EXIT_FAILURE); }
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
			std::cout << temp->size() << std::endl;
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

class Blackjack : Gameboard
{

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
		

