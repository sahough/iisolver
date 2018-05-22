#ifndef HAND_H
#define HAND_H

#include "display.h"

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Hand {

public:
vector<int> cards;
vector<int> ranks;
vector<int> suits;
int type;

Hand(vector<int> _cards) {
        cards = vector<int>(5);
        ranks = vector<int>(5);
        suits = vector<int>(5);
        for(int i = 0; i < 5; i++) {
                cards[i] = _cards[i];
                ranks[i] = cards[i] % 13;
                suits[i] = cards[i] / 13;
        }
        sortRanks();
        type = evaluateType();
}

void sortRanks() {
        vector<int> rankBuckets = vector<int>(13);
        for(int i = 0; i < 5; i++)
                rankBuckets[ranks[i]]++;

        //This section sorts ranks first by frequency, then by value (descending)
        int index = 0;
        for(int freq = 4; freq >= 1; freq--) {
                for(int i = 12; i >= 0; i--) {
                        if(rankBuckets[i] == freq) {
                                rankBuckets[i] = 0;
                                for(int j = 0; j < freq; j++) {
                                        ranks[index] = i;
                                        index++;
                                }
                        }
                }
        }

        //Checks for the wheel, otherwise a wheel would be an "Ace high" straight
        vector<int> wheelRanks = {12,3,2,1,0};
        vector<int> fixedWheelRanks = {3,2,1,0,12};
        if(ranks == wheelRanks)
                ranks = fixedWheelRanks;
}

int evaluateType() {
        //Paired hands
        int pairCount = 0;
        for(int i = 0; i < 5; i++)
                for(int j = i + 1; j < 5; j++)
                        if(ranks[i] == ranks[j])
                                pairCount++;

        switch(pairCount) {
        case 1: return 1; //1 pair
        case 2: return 2; //2 pair
        case 3: return 3; //3 of a kind
        case 4: return 6; //Full house
        case 6: return 7; //4 of a kind
        }

        //Unpaired hands
        bool flush = true;
        bool straight = true;
        for(int i = 0; i < 4; i++)
                if(suits[i]!=suits[i+1])
                        flush = false;
        for(int i = 0; i < 4; i++)
                if(ranks[i]-1!=ranks[i+1])
                        straight = false;
        vector<int> fixedWheelRanks = {3,2,1,0,12};
        if(ranks == fixedWheelRanks)
                straight = true;

        if(!straight && !flush)
                return 0;
        if(straight && !flush)
                return 4;
        if(!straight && flush)
                return 5;
        if(straight && flush)
                return 8;
        return 0;
}

string toString() {
        Display dis;
        string out;
        out += dis.displayCards(cards) + "\n";
        out += dis.displayType(type) + "\n";
        out += dis.displayRanks(ranks);
        return out;
}

bool operator< (const Hand& h) const {
        if (type < h.type) return true;
        if (type > h.type) return false;
        for(int i = 0; i < 5; i++) {
                if(ranks[i] < h.ranks[i]) return true;
                if(ranks[i] > h.ranks[i]) return false;
        }
        return false;
}

bool operator== (const Hand& h) const {
        if (type != h.type) return false;
        for(int i = 0; i < 5; i++) {
                if(ranks[i] != h.ranks[i]) return false;
        }
        return true;
}

};

#endif
