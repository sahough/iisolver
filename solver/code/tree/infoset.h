#ifndef INFOSET_H
#define INFOSET_H

#include "history.h"

class InfoSet {

public:
int handCode;
History h;

InfoSet() {}

InfoSet(int _handCode, History _h) {
        handCode = _handCode;
        h = _h;
}

bool operator< (const InfoSet& is) const {
        if(handCode < is.handCode) return true;
        if(handCode > is.handCode) return false;
        return h < is.h;
}

string toString() {
        string out;
        out += "Hand Code: " + to_string(handCode) + "\n";
        out += h.toString();
        return out;
}

};

#endif
