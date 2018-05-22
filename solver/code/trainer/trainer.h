#ifndef TRAINER_H
#define TRAINER_H

#include "../tree/all.h"
#include "lookuptable.h"
#include "mpihandler.h"
#include "tasker.h"

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <mpi.h>

#include "../game/fileio.h"

using namespace std;

class Trainer {

public:
int F_SIZE;
int T_SIZE;
int R_SIZE;
int C_SIZE;
LookupTable lt;
map<InfoSet,Node> nodeMap;

Tasker tasker;
MpiHandler handler;

Trainer() {
}

Trainer(int _F, int _T, int _R, int _C) {
        lt = LookupTable(_F, _T, _R, _C);
        initFromLookupTable();
}

Trainer(LookupTable _lt) {
        lt = _lt;
        initFromLookupTable();
}

void initFromLookupTable() {
        F_SIZE = lt.F_SIZE;
        T_SIZE = lt.T_SIZE;
        R_SIZE = lt.R_SIZE;
        C_SIZE = lt.C_SIZE;
        tasker = Tasker(F_SIZE);
}

vector<double> CFR(History h, int i, double w, vector<double> pi1) {
        vector<double> u(C_SIZE);
        int stat = h.status();

        //CHANCE ACTION - RIVER
        if(stat == 5) {
                for(int r = 0; r < R_SIZE; r++) {
                        int river = lt.rivers[r];
                        History hr = History(h);
                        hr.board.push_back(river);
                        hr.nextStreet();

                        vector<double> uPrime = CFR(hr, i, w, pi1);
                        for(int c = 0; c < C_SIZE; c++)
                                u[c] += uPrime[c] / (double)R_SIZE;
                }

                return u;
        }

        //CHANCE ACTION - TURN
        if(stat == 4) {
                for(int t = 0; t < T_SIZE; t++) {
                        int turn = lt.turns[t];
                        History ht = History(h);
                        ht.board.push_back(turn);
                        ht.nextStreet();

                        vector<double> uPrime = CFR(ht, i, w, pi1);
                        for(int c = 0; c < C_SIZE; c++)
                                u[c] += uPrime[c] / (double)T_SIZE;
                }

                return u;
        }

        //CHANCE ACTION - FLOP
        if(stat == 3) {

                for(int f = 0; f < F_SIZE; f++) {

                        History hf = History(h);
                        hf.board = lt.flops[f];
                        hf.nextStreet();

                        int worker = tasker.workerForTask(f);

                        //statusFlag
                        handler.sendInt(worker, 0);

                        handler.sendHistory(worker, hf);
                        handler.sendInt(worker, i);
                        handler.sendDouble(worker, w);
                        handler.sendVectorDouble(worker, pi1);
                }
                for(int f = 0; f < F_SIZE; f++) {
                        int worker = tasker.workerForTask(f);

                        vector<double> uPrime = handler.recvVectorDouble(worker);
                        for(int c = 0; c < C_SIZE; c++)
                                u[c] += uPrime[c] / F_SIZE;
                }

                return u;
        }

        //TERMINAL
        if(stat == 2) {
                u = uiShowdown(h, pi1);
                return u;
        }
        if(stat == 1) {
                u = uiFold(h, pi1);
                if(h.currentPlayer() != i)
                        for(int c = 0; c < C_SIZE; c++)
                                u[c] = -u[c];
                return u;
        }

        //PLAYER ACTION

        vector<int> aSet = h.actions();
        int A_SIZE = aSet.size();

        vector<InfoSet> I = lookupInfoSets(h);
        vector<vector<double> > sigma = regretMatching(I, A_SIZE);

        vector<vector<double> > m = vector<vector<double> >(C_SIZE, vector<double>(A_SIZE));

        //ACTION CALLS
        if(h.currentPlayer() == i) {
                for(int a = 0; a < A_SIZE; a++) {
                        History ha = History(h);
                        ha.add(aSet[a]);
                        vector<double> uPrime = CFR(ha, i, w, pi1);

                        for(int c = 0; c < C_SIZE; c++)
                                m[c][a] = uPrime[c];

                        for(int c = 0; c < C_SIZE; c++)
                                u[c] += sigma[c][a] * uPrime[c];
                }
        } else {
                for(int a = 0; a < A_SIZE; a++) {
                        vector<double> pi1prime(C_SIZE);
                        for(int c = 0; c < C_SIZE; c++)
                                pi1prime[c] = sigma[c][a] * pi1[c];

                        History ha = History(h);
                        ha.add(aSet[a]);
                        vector<double> uPrime = CFR(ha, i, w, pi1prime);

                        for(int c = 0; c < C_SIZE; c++)
                                u[c] += uPrime[c];
                }
        }

        //UPDATE REGRET/STRATEGY SUMS
        if(h.currentPlayer() == i) {
                for(int c = 0; c < C_SIZE; c++) {
                        InfoSet is = I[c];
                        Node n = nodeMap[is];
                        for(int a = 0; a < A_SIZE; a++)
                                n.regretSum[a] = max(n.regretSum[a] + m[c][a] - u[c], 0.0);
                        nodeMap[is] = n;
                }
        } else {
                for(int c = 0; c < C_SIZE; c++) {
                        InfoSet is = I[c];
                        Node n = nodeMap[is];
                        for(int a = 0; a < A_SIZE; a++)
                                n.strategySum[a] = n.strategySum[a] + sigma[c][a] * pi1[c] * w;
                        nodeMap[is] = n;
                }
        }

        return u;
}

void printPis(History h) {

        vector<double> blankPi(C_SIZE, 1);

        vector<vector<double> > rs = getReaches(new History(true), h, blankPi, blankPi);
        vector<double> pi0 = rs[0];
        vector<double> pi1 = rs[1];

        cout << fixed;
        cout << setprecision(3);

        cout << "pi0: ";
        for(double d : pi0)
                cout << d << " ";
        cout << endl;

        cout << "pi1: ";
        for(double d : pi1)
                cout << d << " ";
        cout << endl;

}

vector<vector<double> > getReaches(History cur, History target, vector<double> pi0, vector<double> pi1) {

        if(cur == target)
        {
                vector<vector<double> > reaches;
                reaches.push_back(pi0);
                reaches.push_back(pi1);
                return reaches;
        }

        int stat = cur.status();

        //RIVER
        if(stat == 5) {
                cur.board.push_back(target.board[4]);
                cur.nextStreet();
        }

        //TURN
        else if(stat == 4) {
                cur.board.push_back(target.board[3]);
                cur.nextStreet();
        }

        //FLOP
        else if(stat == 3) {
                vector<int> flop;
                flop.push_back(target.board[0]);
                flop.push_back(target.board[1]);
                flop.push_back(target.board[2]);
                cur.board = flop;
                cur.nextStreet();
        }

        //PLAYER ACTION
        else if(stat == 0) {
                int bridge = target.bridgeAction(cur);
                int bridgeIndex = cur.indexOfAction(bridge);

                for(int c = 0; c < C_SIZE; c++) {
                        vector<double> strat = getStrategyAt(c, cur);
                        double bridgeStrat = strat[bridgeIndex];

                        if(cur.currentPlayer() == 0)
                                pi0[c] *= bridgeStrat;
                        else
                                pi1[c] *= bridgeStrat;
                }
                cur.add(bridge);
        }

        return getReaches(cur, target, pi0, pi1);


}

vector<InfoSet> lookupInfoSets(History h) {
        vector<InfoSet> I;
        for(int c = 0; c < C_SIZE; c++)
                I.push_back(InfoSet(c,h));
        return I;
}

vector<vector<double> > regretMatching(vector<InfoSet> I, int A_SIZE) {
        vector<vector<double> > sigma = vector<vector<double> >(C_SIZE, vector<double>(A_SIZE));
        for(int c = 0; c < C_SIZE; c++) {
                InfoSet is = I[c];
                if(nodeMap.count(is) == 0) {
                        Node n = Node(A_SIZE);
                        nodeMap[is] = n;
                }
                Node n = nodeMap[is];
                sigma[c] = n.getStrategy();
        }
        return sigma;
}

vector<double> uiFold(History h, vector<double> pi1) {
        vector<double> u(C_SIZE);

        for(int c0 = 0; c0 < C_SIZE; c0++) {
                double sum = 0.0;
                for(int c1 = 0; c1 < C_SIZE; c1++) {
                        if(c0 != c1) {
                                double util = h.utilFold();
                                double weightedUtil = pi1[c1] * util;
                                sum += weightedUtil;
                        }
                }
                sum /= (C_SIZE - 1);
                u[c0] = sum;
        }

        return u;
}

vector<double> uiShowdown(History h, vector<double> pi1) {
        vector<double> u(C_SIZE);

        for(int c0 = 0; c0 < C_SIZE; c0++) {
                double sum = 0.0;
                for(int c1 = 0; c1 < C_SIZE; c1++) {
                        if(c0 != c1) {
                                double util = h.utilShowdown();
                                int result = lt.resultOn(h.board, c0, c1);
                                if(result == -1)
                                        util = -util;
                                else if(result == 0)
                                        util = 0;
                                double weightedUtil = pi1[c1] * util;
                                sum += weightedUtil;
                        }
                }
                sum /= (C_SIZE - 1);
                u[c0] = sum;
        }

        return u;
}

void train(int T) {

        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        if(world_rank == 0) {
                cout << "Starting train." << endl;
                vector<double> pi1(C_SIZE,1);
                for(int t = 0; t < T; t++) {
                        cout << "Iteration " << t << endl;
                        for(int i = 0; i < 2; i++) {
                                CFR(History(true), i, t, pi1);
                        }
                }
                freeWorkers();
                writeHeadNodeMap();
        }
        else {
                workerWait();
                writeNodeMap();
        }

}

void workerWait() {

        while(true) {

                int statusFlag = handler.recvInt(0);
                if(statusFlag == 999)
                        break;

                History h = handler.recvHistory(0);
                int i = handler.recvInt(0);
                double w = handler.recvDouble(0);
                vector<double> pi1 = handler.recvVectorDouble(0);

                vector<double> uPrime = CFR(h, i, w, pi1);
                handler.sendVectorDouble(0, uPrime);

        }

}

void freeWorkers() {

        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        for(int worker = 1; worker < world_size; worker++)
                handler.sendInt(worker, 999);

        cout << "Freeing all workers." << endl;

}

void writeHeadNodeMap() {

        //cout << "Writing head node map and lookup table." << endl;

        FileIO fio;
        fio.writeNodeMap("data/head.dat", nodeMap);
        fio.writeLookupTable(lt);

}

void writeNodeMap() {

        FileIO fio;
        //cout << "Writing selected items from " << nodeMap.size() << " items." << endl;

        map< vector<int>, map< InfoSet, Node > > boardNodeMaps;

        for(auto &item : nodeMap) {

                InfoSet is = item.first;
                Node n = item.second;
                vector<int> board = is.h.board;

                if(board.size() == 3) { // || board.size() == 4
                        boardNodeMaps[board][is] = n;
                }
        }

        for(auto &item : boardNodeMaps) {

                vector<int> board = item.first;

                string datName = "data/b";
                for(int i : board) {
                        if(i>=10) datName += to_string(i);
                        else datName += "0" + to_string(i);
                }
                datName += ".dat";

                fio.writeNodeMap(datName, item.second);

        }

        //cout << "Wrote all my board node maps." << endl;

}

void subGameTrain(int T, History target) {

        InfoSet is = InfoSet(0, target);
        bool alreadyTrained = nodeMap.count(is);

        if(!alreadyTrained) {

                vector<double> blankPi(C_SIZE, 1);
                vector<vector<double> > rs = getReaches(new History(true), target, blankPi, blankPi);
                vector<double> pi0 = rs[0];
                vector<double> pi1 = rs[1];

                for(int t = 0; t < T; t++) {
                        for(int i = 0; i < 2; i++) {
                                if(i == 0)
                                        CFR(target, i, t, pi1);
                                else
                                        CFR(target, i, t, pi0);
                        }
                }

        }

}

void addStrategy(map<InfoSet, Node> newNodeMap) {
        if(nodeMap.count(newNodeMap.begin()->first) == 0)
                nodeMap.insert(newNodeMap.begin(), newNodeMap.end());
}

vector<double> getStrategyAt(int handCode, History h) {
        InfoSet is = InfoSet(handCode, h);
        Node n = nodeMap[is];
        return n.getAverageStrategy();
}

string toStringDescriptive() {
        string out;
        int terminal = History::TERMINAL_STREET;

        out += "TERMINAL_STREET: ";
        switch(terminal) {
          case 3: out += "FLOP\n"; break;
          case 4: out += "TURN\n"; break;
          case 5: out += "RIVER\n"; break;
          default: out += "INVALID TERMINAL_STREET\n"; break;
        }

        out += "F_SIZE: " + to_string(F_SIZE) + "\n";
        if(terminal >= 4)
        out += "T_SIZE: " + to_string(T_SIZE) + "\n";
        if(terminal >= 5)
        out += "R_SIZE: " + to_string(R_SIZE) + "\n";
        out += "C_SIZE: " + to_string(C_SIZE) + "\n";
        out += "Node Map Size: " + to_string(nodeMap.size()) + "\n";
        return out;
}

string toString() {
        Display dis;
        string out;
        for(auto &item : nodeMap) {
                out += toStringInfoSet(item.first);
                out += item.second.toString() + "\n\n";
        }
        return out;
}

string toStringInfoSet(InfoSet is) {
        Display dis;
        string out;

        //HAND
        out += "Hand: "+lt.range[is.handCode].toString() + "\n";

        //HISTORY
        out += is.h.toString();

        return out;
}

};

#endif
