#ifndef DECK_H
#define DECK_H

#include <vector>
#include <random>

using namespace std;

class Deck {

public:

vector<bool> dealt;
random_device rd;
mt19937 rng;
//Each Deck has its own random device.
//This is acceptable overhead for simplicity.
uniform_int_distribution<int> randCard;

Deck() {
        dealt = vector<bool>(52);
        rng.seed(rd());
        randCard = uniform_int_distribution<int>(0,51);
}

int deal() {
        while(true) {
                int toDeal = randCard(rng);
                if(!dealt[toDeal]) {
                        dealt[toDeal] = true;
                        return toDeal;
                }
        }
}

vector<int> deal(int count) {
        vector<int> ret = vector<int>(count);
        for(int i = 0; i < count; i++)
                ret[i] = deal();
        return ret;
}


};

#endif
