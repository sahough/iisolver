#ifndef GAME_H
#define GAME_H

#include "../trainer/all.h"
#include "../tree/all.h"
#include "fileio.h"
#include <vector>

using namespace std;

class Game {

bool verbose = 0;
bool verboseEngine = 0;

public:

FileIO fio;

Trainer tr;
History h;
vector<int> handCodes;

vector<bool> players;
int score = 0;

bool gameRunning;

random_device rd;
mt19937 rng;


Game() {
        handCodes = vector<int>(2);

        players = vector<bool>(2);
        players[0] = true;
        players[1] = false;

        rng.seed(rd());

        loadTrainer();
}

void loadTrainer() {

        LookupTable lt = fio.readLookupTable();
        tr = Trainer(lt);
        tr.addStrategy(fio.readNodeMap("data/head.dat"));

}

void switchPlayers() {
        players[0] = !players[0];
        players[1] = !players[1];
}

void newHand() {

        gameRunning = true;
        h = History(true);
        switchPlayers();

        uniform_int_distribution<int> uid(0,(tr.C_SIZE)-1);
        int c0 = uid(rng);
        int c1 = c0;
        while(c0 == c1)
                c1 = uid(rng);
        handCodes[0] = c0;
        handCodes[1] = c1;

        update();
}

void update() {
        int stat = h.status();

        if(stat == 5) {
                uniform_int_distribution<int> uid(0,(tr.R_SIZE)-1);
                h.board.push_back(tr.lt.rivers[uid(rng)]);
                h.nextStreet();
                update();
        }
        if(stat == 4) {
                uniform_int_distribution<int> uid(0,(tr.T_SIZE)-1);
                h.board.push_back(tr.lt.turns[uid(rng)]);
                h.nextStreet();
                //Using subgame solver
                cout << "I'm thinking...\n" << endl;
                tr.subGameTrain(100, h);
                update();
        }
        if(stat == 3) {
                uniform_int_distribution<int> uid(0,(tr.F_SIZE)-1);
                h.board = tr.lt.flops[uid(rng)];
                h.nextStreet();
                string datName = "data/b";
                for(int i : h.board) {
                        if(i>=10) datName += to_string(i);
                        else datName += "0" + to_string(i);
                }
                datName += ".dat";
                if(verbose) cout << "Adding new strategy at " << datName << endl;
                tr.addStrategy(fio.readNodeMap(datName));
                update();
        }
        if(stat == 2) {
                gameEndShowdown();
        }
        if(stat == 1) {
                gameEndFold();
        }
        if(stat == 0) {
                if(players[h.currentPlayer()]) {
                        //Human player's turn
                }
                else {
                        engineAction();
                }
        }

}

void engineAction() {
        int enginePlayer = h.currentPlayer();
        int engineHandCode = handCodes[enginePlayer];
        vector<double> strat = tr.getStrategyAt(engineHandCode, h);
        int action = h.actions()[rollAction(strat)]; //Randomly picks valid action
        makeAction(action);
}

int rollAction(vector<double> strat) {
        double sum = 0;
        uniform_real_distribution<double> urd(0,1);
        double roll = urd(rng);
        if(verboseEngine) {
                cout << "Engine Strat" << endl;
                for(int i = 0; i < strat.size(); i++) {
                        cout << to_string(strat[i]) << " ";
                }
                cout << endl;
        }
        for(int i = 0; i < strat.size(); i++) {
                sum += strat[i];
                if(roll < sum)
                        return i;
        }
        return -1; //unreachable
}

void makeAction(int a) {
        h.add(a);
        update();
}

void gameEndShowdown() {
        int util = h.utilShowdown();
        int result = tr.lt.resultOn(h.board, handCodes[0], handCodes[1]);
        if(players[0]) {
                if(result == 0) util = 0;
                if(result == -1) util = -util;
        } else {
                if(result == 0) util = 0;
                if(result == 1) util = -util;
        }
        score += util;
        printWinner(util);
        gameRunning = false;
}

void gameEndFold() {
        int util = h.utilFold();
        if(!players[h.currentPlayer()])
                util = -util;
        score += util;
        printWinner(util);
        gameRunning = false;
}

//OUTPUT
void printWinner(int util) {
        cout << "\n----------\n" << endl;
        if(util < 0)
          cout << "YOU LOSE!" << endl;
        else if(util == 0)
          cout << "SPLIT POT!" << endl;
        else
          cout << "YOU WIN!" << endl;
        cout << "\n----------\n" << endl;
}

string displayLookupTable() {
        return tr.lt.toStringLists();
}

string toString() {
        Display dis;
        string out;

        if(verbose) out += "Node Map Size: " + to_string(tr.nodeMap.size()) + "\n";
        out += "Score: " + to_string(score) + "\n";
        if(players[0])
                out += "Your Hand (BB): " + tr.lt.range[handCodes[0]].toString() + "\n";
        if(players[1])
                out += "Your Hand (SB): " + tr.lt.range[handCodes[1]].toString() + "\n";

        out += h.toString();

        return out;
}

string bothHands() {
        string out;
        out += "BB Hand: " + tr.lt.range[handCodes[0]].toString() + "\n";
        out += "SB Hand: " + tr.lt.range[handCodes[1]].toString() + "\n";
        return out;
}

};

#endif
