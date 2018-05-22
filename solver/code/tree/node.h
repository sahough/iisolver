#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

class Node {

public:
int ACTION_CT;
vector<double> regretSum;
vector<double> strategySum;

Node() {}

Node(int _ACTION_CT) {
        ACTION_CT = _ACTION_CT;
        regretSum = vector<double>(ACTION_CT);
        strategySum = vector<double>(ACTION_CT);
}

vector<double> getStrategy() {
        vector<double> strategy = vector<double>(ACTION_CT);
        for(int i = 0; i < ACTION_CT; i++)
                strategy[i] = regretSum[i] > 0 ? regretSum[i] : 0;
        strategy = normalizeVector(strategy);
        return strategy;
}

vector<double> getAverageStrategy() {
        return normalizeVector(strategySum);
}

vector<double> normalizeVector(vector<double> in) {
        vector<double> out = vector<double>(ACTION_CT);
        double normalSum = 0;
        for(int i = 0; i < ACTION_CT; i++)
                normalSum += in[i];
        if(normalSum > 0) {
                for(int i = 0; i < ACTION_CT; i++)
                        out[i] = in[i] / normalSum;
        } else {
                for(int i = 0; i < ACTION_CT; i++)
                        out[i] = 1.0 / ACTION_CT;
        }
        return out;
}

string toString() {
        stringstream ss;
        ss << fixed << setprecision(6);
        vector<double> avgStrat = getAverageStrategy();
        for(int i = 0; i < ACTION_CT; i++)
                ss << avgStrat[i] << " ";
        return ss.str();
}

};

#endif
