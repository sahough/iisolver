#ifndef TASKER_H
#define TASKER_H

#include <vector>
#include <string>
#include <math.h>
#include <mpi.h>

using namespace std;

class Tasker {

public:

int world_size;
int world_rank;

int F_SIZE;
int taskBlockSize;

Tasker() {}

Tasker(int _F_SIZE) {

        F_SIZE = _F_SIZE;

        //The head node doesn't use Tasker, so we imagine the world is 1 smaller.
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        world_size--;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        world_rank--;

        //This ceiling operation allows for non-divisible tasks.
        //The last worker gets the uneven task.
        //I.E. 5 tasks, 3 workers: [0 has 0,1], [1 has 2,3], [3 has 4]
        double blockSize = (double)F_SIZE / (double)world_size;
        taskBlockSize = (int)ceil(blockSize);

}

int workerForTask(int f) {

        //Adding one so it is the actual node number
        return (f/taskBlockSize)+1;

}

};

#endif
