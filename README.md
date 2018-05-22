## IISolver

IISolver is an imperfect-information game solver which can play simplified versions of 2-player No-Limit Hold'Em, currently only supported on Linux.

It features CFR+, a leading algorithmic approach, and it makes use of distributed computing (i.e. it can be run on supercomputers). It also contains some tools for game abstraction and a basic subgame solver.

## Default Game

IISolver can be adjusted to solve various versions of No-Limit Hold'Em, as described in the abstraction section. The default game is included with the project. 

In this default game, the River is disabled and play stops at the Turn. IISolver can be configured to play the River, but this makes the solution time much longer and the solution files much larger. The other simplifications involved are reductions in the number of possible card runouts, and reductions of the possible bet actions.

## Usage

The executable may need permission to execute:

```
chmod u+x ./iisolver
```

## Play Command

If you want to play against IISolver in the default game:

```
./iisolver play
```

First, there will be a long display of the default game's simplified card runouts:

```
TERMINAL_STREET: TURN
F_SIZE: 215
T_SIZE: 13
C_SIZE: 50
Node Map Size: 400

Node #0
Flops:
2♣ Q♣ 3♦ 
2♣ K♣ 3♥ 
2♣ K♣ 8♠ 
...
```

And then a text-based game session will start:

```
New Hand --------------------

Score: 0
Your Hand (SB): 8♦ 5♥
History:
(Pre-Flop) 0 25 50 
Player Action: 0 50 125 1000 

Action:
```

To input an action, type a number from the "Player Action" options and hit enter. Checking is notated as a bet of 0. Folding is also notated as a bet of 0 when facing action.

## Train Command

In order to do custom training runs, you will need `mpirun`. MPI (Message Passing Interface) is used for parallel computing. See dependencies section if needed.

Before you carry out a training run, you should clear out solution files in /data:
```
make rmdata
```

The syntax for a training run is:
```
mpirun -np N ./iisolver train F T R C I
```
There are 6 parameters (N, F, T, R, C, I).

N is the number of threads used for MPI, one of which is the head. For example, on an 8-thread computer, use 8, and there will be 1 head and 7 workers. For a distributed system, use the total number of threads available. N must be 2 or greater.

F, T, and R are the Flop, Turn, and River sample sizes respectively. C is the hole cards sample size. See abstraction section for details. (T & R can be set to 1 if unused.)

I is the iteration count (the number of times CFR+ runs). The solution quality will increase with the iteration count. 100 is a reasonable starting value for small abstractions.

After this output:
```
Board maps written.
```
The train is finished, and you can now play against it with the play command.

## Abstraction

Aside from disabling the River, there are two methods of simplification used in IISolver. One is simplifying chance actions (hole cards and the board), and the other is simplifying player actions (bet sizes).

##### Chance Action Abstraction

Chance actions are simplified by use of sampling. For example, an extremely simple game could be played on only 3 flops, 3 turns, and 3 rivers, with 10 possible preflop holdings. Such a game is outlined here:

```
F_SIZE: 3
T_SIZE: 3
R_SIZE: 3
C_SIZE: 10
Node Map Size: 80

Node #0
Flops:
A♣ 3♦ 2♥ 
3♦ T♦ J♥ 
5♦ 8♠ J♠ 
Turns:
9♣
6♦
9♠
Rivers:
T♣
9♠
A♣

Range:
4♣ 4♦
6♠ 2♠
7♦ 2♠
8♣ 3♦
8♥ 5♠
9♠ 6♥
T♦ 4♣
J♣ 6♠
J♦ 7♥
A♦ J♦
```

IISolver does not account for card removal - for example a player could hold [8♣ 3♦] on a  board of [A♣ 3♦ 2♥] where the [3♦] is present twice. In future work, it may be possible to retroactively prune impossible boards, or to construct a better abstraction to begin with.

##### Player Action Abstraction

Because there are a huge number of possible actions, especially in No-Limit, the bet sizes are discretized. For example, with 100 big blinds behind, there are 101 possible integer bet sizes. If we instead allow for only {0, 10, 30, 100}, these representative bet sizes can still be a reasonable abstraction.

Betting structure is specified in *code/tree/history.h*. Any betting structure can be set, including pot-based sizing, but not all structures will be practical. The preflop structure is set in the History constructor, while the streets are set in `vector<int> calculateStructure(int street)`.

##### Enabling River

To enable River play, change `TERMINAL_STREET` in *code/tree/history.h* to 5. This will require re-training. Also, this will greatly expand the size of the game, so you may want to use other simplifications.

## Dependencies / Compilation

Included in the project folder are some Boost libraries - the only ones that are actually used are the serialization and archive libraries. 

The `mpic++` compiler is also needed, available at http://www.mpich.org/downloads/. (OpenMPI should also be usable). The C++ version is `-std=gnu++11`. To compile, simply `make`.

## Example Distributed Run

Here is an example of a distributed run shell script. You may need to load a MPI module. Note the 1 for the River parameter indicates the River is unused.
```
#PBS -N IISolver
#PBS -l nodes=6:ppn=36
#PBS -l walltime=168:00:00

module load mpich/gnu/3.2
mpirun -np 216 ./iisolver train 215 13 1 50 100
```

## Contact

If you have any questions or if you'd like to work together, feel free to send me an email. I can explain the notation & algorithms as well as the implementation details. I also have some training results from larger game abstractions.
Email: stephen.a.hough@gmail.com
