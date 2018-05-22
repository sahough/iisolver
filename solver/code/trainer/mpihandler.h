#ifndef MPIHANDLER_H
#define MPIHANDLER_H

#include "../tree/all.h"
#include "mpi.h"

#include <vector>

class MpiHandler {

public:

//Helper methods block
//Send/Recv:
//Primitives: int, double
//Vector of Primitives: vector<int>, vector<double>

void sendInt(int dest, int i) {

        MPI_Send(&i, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);

}

int recvInt(int src) {

        int i;
        MPI_Recv(&i, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        return i;

}

void sendDouble(int dest, double d) {

        MPI_Send(&d, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

}

int recvDouble(int src) {

        double d;
        MPI_Recv(&d, 1, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        return d;

}

void sendVectorInt(int dest, vector<int> vec) {

        int vecSize = vec.size();
        MPI_Send(&vecSize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        if(vecSize > 0)
                MPI_Send(&vec[0], vecSize, MPI_INT, dest, 0, MPI_COMM_WORLD);

}

vector<int> recvVectorInt(int src) {

        int vecSize;
        MPI_Recv(&vecSize, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        vector<int> vec(vecSize);
        if(vecSize > 0)
                MPI_Recv(&vec[0], vecSize, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        return vec;

}

void sendVectorDouble(int dest, vector<double> vec) {

        int vecSize = vec.size();
        MPI_Send(&vecSize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        if(vecSize > 0)
                MPI_Send(&vec[0], vecSize, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

}

vector<double> recvVectorDouble(int src) {

        int vecSize;
        MPI_Recv(&vecSize, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        vector<double> vec(vecSize);
        if(vecSize > 0)
                MPI_Recv(&vec[0], vecSize, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        return vec;

}

//Object block

void sendSubHistory(int dest, SubHistory sh) {

        sendVectorInt(dest, sh.line);
        sendVectorInt(dest, sh.structure);

}

SubHistory recvSubHistory(int src) {

        SubHistory sh;
        sh.line = recvVectorInt(src);
        sh.structure = recvVectorInt(src);
        return sh;

}

void sendHistory(int dest, History h) {

        sendInt(dest, h.streets.size());
        for(int i = 0; i < h.streets.size(); i++) {
                sendSubHistory(dest, h.streets[i]);
        }
        sendVectorInt(dest, h.board);

}

History recvHistory(int src) {

        History h;
        int streetsSize = recvInt(src);
        for(int i = 0; i < streetsSize; i++) {
                SubHistory sh = recvSubHistory(src);
                h.streets.push_back(sh);
        }
        h.board = recvVectorInt(src);
        return h;

}

void sendInfoSet(int dest, InfoSet is) {

        sendInt(dest, is.handCode);
        sendHistory(dest, is.h);

}

InfoSet recvInfoSet(int src) {

        int handCode = recvInt(src);
        History h = recvHistory(src);
        InfoSet is = InfoSet(handCode, h);
        return is;

}

void sendNode(int dest, Node n) {

        sendInt(dest, n.ACTION_CT);
        sendVectorDouble(dest, n.regretSum);
        sendVectorDouble(dest, n.strategySum);

}

Node recvNode(int src) {

        int ACTION_CT = recvInt(src);
        Node n = Node(ACTION_CT);
        n.regretSum = recvVectorDouble(src);
        n.strategySum = recvVectorDouble(src);
        return n;

}

};

#endif
