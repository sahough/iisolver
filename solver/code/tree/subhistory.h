#ifndef SUBHISTORY_H
#define SUBHISTORY_H

#include <vector>
#include <string>

using namespace std;

class SubHistory {

public:
vector<int> line;
vector<int> structure;

SubHistory() {}

SubHistory(vector<int> _structure) {
        structure = _structure;
}

SubHistory(const SubHistory &sh) {
        line = sh.line;
        structure = sh.structure;
}

//ACCESS
int size() {
        return line.size();
}

int currentPlayer() {
        return size() % 2;
}

int safeGet(int index) {
        if(index < 0 || index >= size())
                return -999;
        return line[index];
}

void add(int a) {
        line.push_back(a);
}

//ACTION
vector<int> actions() {
        if(size() == 0)
                return structure;
        int last = safeGet(size() - 1);
        if(last == 0)
                return structure;
        vector<int> aset;
        aset.push_back(0);
        for(int i : structure)
                if(i >= last)
                        aset.push_back(i);
        return aset;
}

int status() {
        int last = safeGet(size() - 1);
        int preLast = safeGet(size() - 2);
        if(last == -999 || preLast == -999)
                return 0;
        if(last == preLast)
                return 2;
        if(last == 0 && preLast > 0)
                return 1;
        return 0;
}

//UTILITY
int utilShowdown() {
        return safeGet(size() - 1);
}

int utilFold() {
        int sum = 0;
        if(size() >= 3)
                sum += safeGet(size() -3);
        return sum;
}

//OUTPUT
string toString() {
        string out;
        for(int i : line)
                out += to_string(i) + " ";
        return out;
}

//OPERATOR
bool operator< (const SubHistory& sh) const {
        return line < sh.line;
}

bool operator== (const SubHistory& sh) const {
        return line == sh.line;
}

};

#endif
