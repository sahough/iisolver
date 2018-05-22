#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H

#include "../poker/all.h"

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include <mpi.h>

class LookupTable {

public:

int F_SIZE;
int T_SIZE;
int R_SIZE;
int C_SIZE;

Evaluator e;
vector<vector<int> > flops;
vector<int> turns;
vector<int> rivers;
vector<Pocket> range;
map<vector<int>,vector<vector<int> > > boardMap;

LookupTable() {
}

//This constructor synchronizes the turn, river, and range across all nodes.
//The flops are passed by CFR.
LookupTable (int _F, int _T, int _R, int _C) {
        F_SIZE = _F;
        T_SIZE = _T;
        R_SIZE = _R;
        C_SIZE = _C;

        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        if(world_rank == 0) {
                rangeGenerator(C_SIZE);
                vector<int> rangeData;
                for(Pocket p : range) {
                        rangeData.push_back(p.cards.first);
                        rangeData.push_back(p.cards.second);
                }
                flopGenerator(F_SIZE);
                turnGenerator(T_SIZE);
                riverGenerator(R_SIZE);

                for(int w = 1; w < world_size; w++) {
                        MPI_Send(&rangeData[0], C_SIZE*2, MPI_INT, w, 0, MPI_COMM_WORLD);
                        MPI_Send(&turns[0], T_SIZE, MPI_INT, w, 0, MPI_COMM_WORLD);
                        MPI_Send(&rivers[0], R_SIZE, MPI_INT, w, 0, MPI_COMM_WORLD);
                }
        }
        else {
                vector<int> rangeData(C_SIZE*2);
                MPI_Recv(&rangeData[0], C_SIZE*2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for(int i = 0; i < C_SIZE; i++) {
                        vector<int> cards = {rangeData[i*2],rangeData[i*2+1]};
                        Pocket p = Pocket(cards);
                        range.push_back(p);
                }
                turns = vector<int>(T_SIZE);
                rivers = vector<int>(R_SIZE);
                MPI_Recv(&turns[0], T_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&rivers[0], R_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

}

void flopGenerator(int size) {
        while(flops.size() < size) {
                Deck d;
                vector<int> flop = d.deal(3);
                sort (flop.begin(), flop.end());

                if(find(flops.begin(), flops.end(), flop) == flops.end())
                        flops.push_back(flop);
        }
        sort(flops.begin(), flops.end());
}

void turnGenerator(int size) {
        //Special sampling option which is practically close to 52-samples
        if(size == 13) {
                turns = {0,1,2,3,17,18,19,33,34,35,49,50,51};
        }
        else {
                Deck d;
                for(int i = 0; i < size; i++) {
                        turns.push_back(d.deal());
                }
        }
}

void riverGenerator(int size) {
        //Special sampling option which is practically close to 52-samples
        if(size == 13) {
                rivers = {39,40,41,42,30,31,32,20,21,22,10,11,12};
        }
        else {
                Deck d;
                for(int i = 0; i < size; i++) {
                        rivers.push_back(d.deal());
                }
        }
}

void rangeGenerator(int size) {
        //Full range
        if(size == 1326) {
                for(int c0 = 0; c0 < 51; c0++) {
                        for(int c1 = c0+1; c1 < 52; c1++) {
                                Pocket p = Pocket({c0, c1});
                                range.push_back(p);
                        }
                }
        }
        else {
                for(int i = 0; i < size; i++) {
                        Deck d;
                        Pocket p = Pocket(d.deal(2));
                        range.push_back(p);
                }
                sort(range.begin(), range.end());
        }
}

int resultOn(vector<int> board, int c0, int c1) {

        //Populate the hand matrix if not found
        if(boardMap.count(board) == 0) {
                vector<vector<int> > results = vector<vector<int> >(C_SIZE, vector<int>(C_SIZE));
                for(int h0 = 0; h0 < C_SIZE; h0++) {
                        for(int h1 = 0; h1 < C_SIZE; h1++) {
                                if(h0 != h1) {
                                        Pocket hand0 = range[h0];
                                        Pocket hand1 = range[h1];
                                        int result = e.compareHandOnBoard(board, hand0, hand1);
                                        results[h0][h1] = result;
                                }
                        }
                }
                boardMap[board] = results;
        }

        return boardMap[board][c0][c1];

}

string toStringLists() {
        Display dis;
        string out;
        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        out += "Node #" + to_string(world_rank) + "\n";

        out += "Flops:\n";
        for(vector<int> i : flops) {
                out += dis.displayCards(i) + "\n";
        }

        if(History::TERMINAL_STREET >= 4) {
        out += "Turns:\n";
        for(int i : turns) {
                out += dis.displayCard(i) + "\n";
        }
        }

        if(History::TERMINAL_STREET >= 5) {
        out += "Rivers:\n";
        for(int i : rivers) {
                out += dis.displayCard(i) + "\n";
        }
        }

        out += "\nRange:\n";
        for(Pocket p : range) {
                out += p.toString() + "\n";
        }
        return out;
}

};

#endif
