#include<iostream>

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
			std::string name;
			int numCards;

			Player() { hand = new Card[MAX_PER_SUIT], name = "", numCards = 0; }
			Player(std::string name) : hand(new Card[MAX_PER_SUIT]), name(name), numCards(0) {}
		};

		size_t numPlayers;
		Card* deck;
		const std::string suits[4] = {"HEARTS","DIAMONDS","CLUBS","SPADES"};

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
			int suitLimit = MAX_PER_SUIT;
			int currSuit = 0;
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

	public:
		Gameboard(int numPlayers) {
			this->numPlayers = numPlayers;
			deck = new Card[MAX_CARDS];
			setDeck();
		}

		void deal()
		{
			//TO DO: SHUFFLE, DEAL
		}
		
		void viewDeck()
		{
			for (size_t i = 0; i < MAX_CARDS; i++)
			{
				std::string str_rank = rankToString(deck[i].rank);
				std::cout << str_rank << " of " << deck[i].suit << std::endl;
			}
			
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
		

