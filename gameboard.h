#include<iostream>
#include<set>

using namespace std;

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

class Player
{
	private:
		struct Card {
			
			int rank;
			string suit;
		
			Card() {rank = 0; suit = "";}
			Card(int rank, string suit) : rank(rank) , suit(suit) {}
		};
		
		Card* hand;
		size_t numCards;

	public:
		Player()
		{
			hand = new Card[13];
			numCards = 0;
		}

		void displayCards()
		{
			for(size_t i = 0; i < numCards; i++)
			{
				cout << hand[i].rank << " of " << hand[i].suit << endl;
			}
		}
};


#endif
		

