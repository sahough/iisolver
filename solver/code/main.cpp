#include "poker/all.h"
#include "tree/all.h"
#include "trainer/all.h"
#include "game/all.h"

#include <cstdlib>
#include <iostream>
#include <random>
#include <map>
#include <chrono>
#include <limits>
#include <mpi.h>

using namespace std;

//PRECONDITION:
//lookuptable.dat loaded
//head.dat loaded
//bXXXXXX.dat all loaded (count = F_SIZE)

void playGame() {
        Game g;

        cout << g.tr.toStringDescriptive() << endl;
        cout << g.tr.lt.toStringLists() << endl;
        //cout << g.tr.toString() << endl;

        g.newHand();
        bool sessionRunning = true;

        while(sessionRunning) {

                cout << "New Hand --------------------\n" << endl;
                while(g.gameRunning) {
                        cout << g.toString() << endl;

                        vector<int> aSet = g.h.actions();
                        if(aSet.size() == 1) {
                            //If we only have 1 action, in which case we should take it (used for All-Ins)
                            g.makeAction(aSet[0]);
                        }
                        else {

                        //Get the input and check if it is an integer
                        cout << "Action: ";

                        int action;
                        cin >> action;
                        while(cin.fail()) {
                            cout << "Invalid action, non-integer input." << std::endl;
                            cin.clear();
                            cin.ignore(256,'\n');
                            cin >> action;
                        }

                        //If the input is an integer, see if it is in the action set
                        bool isValidAction = find(aSet.begin(), aSet.end(), action) != aSet.end();
                        if(isValidAction) {
                                g.makeAction(action);
                                cin.clear();
                                cin.sync();
                        }
                        else
                                cout << "Please select from the Player Actions.\n" << endl;

                        }

                }

                cout << "Hand Over:" << endl;
                cout << g.toString() << endl;
                cout << g.bothHands() << endl;
                cout << "\n----------------------------- " << endl;
                cout << "Press Enter to Continue";
                cout << "\n-----------------------------" << endl;

                cin.ignore();
                cin.get();

                cout <<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << endl;

                g.newHand();
        }

}

int main(int argc, char** argv) {

        MPI_Init(NULL, NULL);
        int world_size;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        int world_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

        string mode = argv[1];

        //WRITE MODE
        if(mode == "train") {

                int fSize = atoi(argv[2]);
                int tSize = atoi(argv[3]);
                int rSize = atoi(argv[4]);
                int cSize = atoi(argv[5]);
                int iter = atoi(argv[6]);

                //TRAIN PHASE

                Trainer tr = Trainer(fSize,tSize,rSize,cSize);
                auto start = chrono::system_clock::now();
                tr.train(iter);
                auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - start);



                if(world_rank == 0) {
                        cout << endl;
                        cout << "Train Time (total) (ms): " << elapsed.count() << endl;

                        cout << tr.toStringDescriptive() << endl;
                        cout << "Board maps written." << endl;

                }

        }

        //READ + PRINT MODE
        else if(mode == "play") {
                playGame();
        }

        MPI_Finalize();
        return 1;

}
