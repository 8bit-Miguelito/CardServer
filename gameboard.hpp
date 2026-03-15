#include<iostream>
#include<vector>

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

const int MAX_PER_SUIT = 13;
const int MAX_CARDS = 52;

using namespace std;

class Gameboard 
{
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
		//methods
		virtual void deal() = 0;

		inline void addToHand(Player*& player, const Card& card)
		{
			player->numCards++;
			player->hand.insert(card);
		}
		
		//variables 
		size_t numPlayers;
		Card* deck;
		vector<Player*> players;


	public:
		Gameboard(const set<int>& players) {
			this->numPlayers = players.size();
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
	public:
		Blackjack(const set<int>& players) : Gameboard(players) {}

		size_t dealt;
		void deal() override
		{
			dealt = 0;
			int currPlayer = 0;
			for (size_t i = dealt; i < MAX_CARDS; i++)
			{
				if((currPlayer % numPlayers) == 0) currPlayer = 0;
				addToHand(players.at(currPlayer), deck[i]);
			}
		}
};



#endif