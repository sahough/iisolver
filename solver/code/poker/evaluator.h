#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "pocket.h"
#include "hand.h"
#include <vector>

using namespace std;

class Evaluator {

public:

vector<vector<int> > chooseFromPool(vector<int> pool) {
        vector<vector<int> > combos;

        int n = pool.size();
        if(n == 5) {
                combos.push_back(pool);
        } else if (n == 6) {
                for(int remove1 = 0; remove1 < n; remove1++) {
                        vector<int> combo;
                        for(int i = 0; i < n; i++)
                                if(i != remove1) combo.push_back(pool[i]);
                        combos.push_back(combo);
                }
        } else if (n == 7) {
                for(int remove1 = 0; remove1 < n; remove1++) {
                        for(int remove2 = 0; remove2 < n; remove2++) {
                                vector<int> combo;
                                for(int i = 0; i < n; i++)
                                        if(i != remove1 && i != remove2) combo.push_back(pool[i]);
                                combos.push_back(combo);
                        }
                }
        }
        return combos;
}

Hand bestHandOnBoard(vector<int> board, Pocket a) {
        vector<int> pool = board;
        pool.push_back(a.cards.first);
        pool.push_back(a.cards.second);

        vector<vector<int> > combos = chooseFromPool(pool);
        vector<Hand> hands;
        for(int i = 0; i < combos.size(); i++) {
                hands.push_back(Hand(combos[i]));
        }
        Hand maxHand = *max_element(hands.begin(), hands.end());

        return maxHand;
}

int compareHandOnBoard(vector<int> board, Pocket a, Pocket b) {
        Hand aBest = bestHandOnBoard(board, a);
        Hand bBest = bestHandOnBoard(board, b);
        if(aBest == bBest) return 0;
        if(aBest < bBest) return -1;
        return 1;
}

};

#endif
