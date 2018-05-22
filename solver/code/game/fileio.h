#ifndef FILEIO_H
#define FILEIO_H

#include <fstream>
#include <boost/serialization/map.hpp>
#include <map>

#include "serializer.h"
#include "../tree/all.h"

class FileIO {

public:

void writeLookupTable(LookupTable lt) {
        {
                ofstream outFileStream("data/lookuptable.dat");
                boost::archive::text_oarchive outArchive(outFileStream);
                outArchive << lt;
        }
}

LookupTable readLookupTable() {
        LookupTable lt;
        {
                ifstream inFileStream("data/lookuptable.dat");
                boost::archive::text_iarchive inArchive(inFileStream);
                inArchive >> lt;
        }
        return lt;
}

void writeNodeMap(string nodeMapName, map<InfoSet,Node> nodeMap) {
        {
                ofstream outFileStream(nodeMapName);
                boost::archive::text_oarchive outArchive(outFileStream);
                outArchive << nodeMap;
        }
}

map<InfoSet, Node> readNodeMap(string nodeMapName) {
        map<InfoSet, Node> nodeMap;
        {
                ifstream inFileStream(nodeMapName);
                boost::archive::text_iarchive inArchive(inFileStream);
                inArchive >> nodeMap;
        }
        return nodeMap;
}


};

#endif
