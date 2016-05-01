#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <vector>

#include "Board.h"
#include "Dictionary.h"

//Customize to match number of processors
const int NUM_THREADS = 4;
//Changes granularity of workQueue items, use for larger and sparser boards
const bool BY_ROW = false;

struct Search
{
    Search(Dictionary* dictionary, DictionaryNode* dNode, const Board* board, unsigned int bIndex);
    ~Search();

    Dictionary* mDictionary;
    DictionaryNode* mDNode;
    const Board* mBoard;
    unsigned int mBIndex;
    std::vector<unsigned int>* mVisited;
};

//TODO: Make this a template class once we figure out how std::thread uses va_list or whatever
class SolverThreadPool
{
public:
    //Give the function whose last argument is of type T (corresponding to the work queue)
    SolverThreadPool(void(*fnc)(Search *), int numWorkItems);

    //Must fill stack with tasks before starting, not smart enough to work with dynamically produced work
    void start(Dictionary* dictionary, const Board* board);
    void join();

private:
    static void startSolverWorker(Dictionary* dictionary, const Board* board);

    static std::thread mThreads[NUM_THREADS];

    static void(*mFnc)(Search *);
    //SolverThreadPool doesn't manage its own work queue since it is passed in
    //TODO: Figure out a way to store data types as something other than pointers
    static std::atomic<int> mNumWorkItems;
};

class Solver
{
public:
    //TODO: If we re-use this dictionary, either reset it or don't modify it
    Solver(Dictionary* dictionary, const Board* board, const std::string filename);
    ~Solver();

    void solve();

private:
    //Checks if it's a valid word that has not been found yet
    static void checkSearch(Search* search);
    //Depth-first search the calls checkSearch at every step
    static void recursiveSearch(Search* search);
    //Checks if an index is in the visited index list
    static inline bool indexVisited(unsigned int bIndex, std::vector<unsigned int>* visited);

    SolverThreadPool* mThreadPool;
    Dictionary* mDictionary;
    const Board* mBoard;
    static FILE* solverOutfile;
};
