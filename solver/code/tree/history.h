#ifndef HISTORY_H
#define HISTORY_H

#include "subhistory.h"
#include "../poker/display.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;

class History {

public:

//Terminal street is the last street that should be taken into account in abstraction
//Must be set to 3, 4, or 5
//3 - Flop
//4 - Turn
//5 - River
static const int TERMINAL_STREET = 4;


int sb = 25;
int bb = 50;
int stackSize = 1000;

vector<SubHistory> streets;
vector<int> board;

History() {
}

//PREFLOP CONSTRUCTOR
//Blank is unused
History(bool blank) {

        //vector<int> preFlopStructure = {0,sb,bb,2*bb};
        //vector<int> preFlopStructure = {0,sb,bb,5*bb/2,9*bb,21*bb,stackSize};
        vector<int> preFlopStructure = {0,sb,bb,5*bb/2,stackSize};
        SubHistory preFlop = SubHistory(preFlopStructure);
        preFlop.add(0);
        preFlop.add(sb);
        preFlop.add(bb);
        streets.push_back(preFlop);
}

//COPY CONSTRUCTOR
History(const History &h) {
        streets = h.streets;
        board = h.board;
}

//ACCESS
SubHistory& activeStreet() {
        return streets[streets.size()-1];
}

void add(int a) {
        activeStreet().add(a);
}

int currentPlayer() {
        return activeStreet().currentPlayer();
}

//ACTION
int status() {
        if(hasOption())
                return 0;
        int status = activeStreet().status();
        if(status == 2) {
                //This section determines which street the game ends on (Flop, Turn, or River)
                if(streets.size() == 1) return 3;  //FLOP STATUS
                if(streets.size() == 2 && TERMINAL_STREET >= 4) return 4;  //TURN STATUS
                if(streets.size() == 3 && TERMINAL_STREET >= 5) return 5;  //RIVER STATUS
        }
        return status;
}

bool hasOption() {
        return (streets.size() == 1 && activeStreet().size() == 4 && activeStreet().line[3] == bb);
}

vector<int> actions() {
        vector<int> aSet = activeStreet().actions();
        if(hasOption())
                aSet.erase(aSet.begin());
        return aSet;
}

//Precondition:
//The other History is on a player action
int bridgeAction(History other) {

        int lastStreet = other.streets.size()-1;
        int lastStreetIndex = other.streets[lastStreet].size();

        int bridge = streets[lastStreet].line[lastStreetIndex];
        return bridge;

}

//Used to backtrace from action to index
int indexOfAction(int action) {
        vector<int> aSet = actions();
        for(int i = 0; i < aSet.size(); i++) {
                if(aSet[i] == action)
                        return i;
        }
        return -999;
}

void nextStreet() {
        streets.push_back(SubHistory(calculateStructure(streets.size())));
}

vector<int> calculateStructure(int street) {

        int halfPot = utilShowdown();
        int allIn = stackSize - halfPot;
        int psb = 2 * halfPot;

        vector<int> newStructure;
        //if(streets.size() == 1) {
                newStructure = {0, psb/4*3, 3*psb, allIn};
                vector<int> legalStructure;

                //This block makes sure structure {0,0} is never allowed
                for(int i : newStructure) {
                        if(i < allIn)
                                legalStructure.push_back(i);
                }
                legalStructure.push_back(allIn);

                return legalStructure;

        //}
        /*
        else {

                newStructure = {0, psb/4*3, allIn};
                vector<int> legalStructure;

                //This block makes sure structure {0,0} is never allowed
                for(int i : newStructure) {
                        if(i < allIn)
                                legalStructure.push_back(i);
                }
                legalStructure.push_back(allIn);

                return legalStructure;

                /*  int singleBet = psb/4*3;
                   if(allIn == 0)
                          return {0};
                   else if(singleBet<allIn)
                          return {0,singleBet};
                   else
                          return {0,allIn};*/
        //}

    // for simple structure
    /*  vector<int> newStructure;
      newStructure = {0,200};
      return newStructure;*/

}

//UTILITY
int potSize() {
        int pot = 0;
        for(int i = 0; i < streets.size() - 1; i++) {
                SubHistory sh = streets[i];
                pot += sh.safeGet(sh.size()-1);
        }
        return pot;
}

int utilFold() {
        return potSize() + activeStreet().utilFold();
}

int utilShowdown() {
        return potSize() + activeStreet().utilShowdown();
}

//OUTPUT
string toString() {
        Display dis;
        string out = "History:\n";
        if(board.size() > 0) {
                out += dis.displayCards(board) + "\n";
        }
        for(int i = 0; i < streets.size(); i++) {
                switch(i) {
                case 0: out += "(Pre-Flop) "; break;
                case 1: out += "(Flop)     "; break;
                case 2: out += "(Turn)     "; break;
                case 3: out += "(River)    "; break;
                }
                out += streets[i].toString() + "\n";
        }
        /*out += "Structure:\n";
           for(int i = 0; i < streets.size(); i++) {
                vector<int> shStructure = streets[i].structure;
                for(int i : shStructure)
                        out += to_string(i) + " ";
                out += "\n";
           }*/
        out += statusString();
        out += "\n";
        return out;
}

string statusString() {
        int stat = status();
        string out;
        switch(stat) {
        case 0: { out += "Player Action: ";
                  vector<int> aSet = actions();
                  for(int i : aSet)
                          out += to_string(i) + " ";
                  break; }
        case 1: out += "Terminal Fold: ";
                out += to_string(utilFold());
                break;
        case 2: out += "Terminal Showdown: ";
                out += to_string(utilShowdown());
                break;
        case 3: out += "Chance Action: ";
                break;
        }
        return out;
}

//OPERATOR
bool operator< (const History& h) const {
        if(streets.size() < h.streets.size()) return true;
        if(streets.size() > h.streets.size()) return false;
        if(streets < h.streets) return true;
        if(streets > h.streets) return false;
        if(board < h.board) return true;
        if(board > h.board) return false;
        return false;
}

bool operator== (const History& h) const {
        if(streets.size() != h.streets.size()) return false;
        if(streets != h.streets) return false;
        if(board != h.board) return false;
        return true;
}

};

#endif
