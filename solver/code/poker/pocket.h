#ifndef POCKET_H
#define POCKET_H

#include "display.h"

#include <utility>

using namespace std;

class Pocket {

public:

pair<int,int> cards;

Pocket() {}

Pocket(vector<int> _cards) {
        cards.first = _cards[0];
        cards.second = _cards[1];
        if(cards.first % 13 < cards.second % 13) {
                int temp = cards.first;
                cards.first = cards.second;
                cards.second = temp;
        }
}

string toString() {
        Display dis;
        return dis.displayCard(cards.first) + " " +dis.displayCard(cards.second);
}

bool operator< (const Pocket& p) const {
        if (cards.first%13 < p.cards.first%13) return true;
        if (cards.first%13 > p.cards.first%13) return false;
        if (cards.second%13 < p.cards.second%13) return true;
        if (cards.second%13 > p.cards.second%13) return false;

        return false;
}

};

#endif
