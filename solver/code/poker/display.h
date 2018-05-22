#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <vector>

using namespace std;

class Display {

vector<string> ranks = {"2","3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "A" };
vector<string> suits = { "♣", "♦", "♥", "♠" };
vector<string> types = { "High Card", "One Pair", "Two Pair", "Three Of A Kind", "Straight", "Flush", "Full House",
                         "Four Of A Kind", "Straight Flush" };

public:

string displayRank(int rank) {
        return ranks[rank];
}

string displaySuit(int suit) {
        return suits[suit];
}

string displayType(int type) {
        return types[type];
}

string displayCard(int card) {
        return displayRank(card % 13) + displaySuit(card / 13);
}

string displayCards(vector<int> cards) {
        string out;
        for(int i = 0; i < cards.size(); i++)
                out += displayCard(cards[i]) + " ";
        return out;
}

string displayRanks(vector<int> in) {
        string out = "[";
        for(int i = 0; i < in.size(); i++) {
                out += displayRank(in[i]);
                if(i != in.size()-1)
                        out += " ";
        }
        out += "]";
        return out;
}

};

#endif
