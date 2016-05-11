#pragma once

#include <atomic>
#include <string>
#include <thread>
#include <vector>

#include "Board.h"
#include "Dictionary.h"

//Customize to match number of processors
const int NUM_THREADS = 16;
//Changes granularity of workQueue items, use for larger and sparser boards
const bool BY_ROW = false;

struct Search;

using ThreadFunction = void(*)(Search *);

struct Search
{
    Search(Dictionary& dictionary, DictionaryNode* dNode, const Board& board, unsigned int bIndex);
    ~Search();

    DictionaryNode* mDNode;
    unsigned int mBIndex;
    const Board& mBoard;
    std::vector<unsigned int> mVisited;
    Dictionary& mDictionary;
};

//TODO: Make this a template class once we figure out how std::thread uses va_list or whatever
class SolverThreadPool
{
public:
    //Give the function whose last argument is of type T (corresponding to the work queue)
    SolverThreadPool(ThreadFunction fnc, int numWorkItems);

    //Must fill stack with tasks before starting, not smart enough to work with dynamically produced work
    void start(Dictionary& dictionary, const Board& board);
    void join();

private:
    static void startSolverWorker(SolverThreadPool* threadPool, Dictionary* dictionary, const Board* board);

    std::thread mThreads[NUM_THREADS];

    ThreadFunction mFnc;
    //SolverThreadPool doesn't manage its own work queue since it is passed in
    std::atomic<int> mNumWorkItems;
};

class Solver
{
public:
    //TODO: If we re-use this dictionary, either reset it or don't modify it
    Solver(Dictionary& dictionary, const Board& board);
    ~Solver();

    void solve();

private:
    //Depth-first search the calls checkSearch at every step
    static void recursiveSearch(Search* search);
    //Checks if an index is in the visited index list
    static inline bool indexVisited(unsigned int bIndex, std::vector<unsigned int>& visited);

    Dictionary& mDictionary;
    const Board& mBoard;
    SolverThreadPool mThreadPool;
};
