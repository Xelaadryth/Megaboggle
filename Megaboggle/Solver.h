#pragma once

#include "Board.h"
#include "Dictionary.h"
#include <list>
#include <string>
#include <thread>

const int NUM_THREADS = 4;

struct Search
{
    Search(Dictionary* dictionary, DictionaryNode* dNode, const Board* board, unsigned int bIndex, std::list<unsigned int>* visited, unsigned int threadNum);

    Dictionary* mDictionary;
    DictionaryNode* mDNode;
    const Board* mBoard;
    unsigned int mBIndex;
    std::list<unsigned int>* mVisited;
    //TODO: For now does nothing, use for synchronizing writes later
    unsigned int mThreadNum;
};

class Solver
{
public:
    //TODO: If we re-use this dictionary, either reset it or don't modify it
    Solver(Dictionary* dictionary, const Board* board, const std::string filename);
    ~Solver();

    void solve();

private:
    static void startSearch(Dictionary* dictionary, const Board* board, unsigned int threadNum);
    //Checks if it's a valid word that has not been found yet
    static void checkSearch(Search* search);
    //Depth-first search the calls checkSearch at every step
    static void recursiveSearch(Search* search);
    //Checks if an index is in the visited index list
    static inline bool indexVisited(unsigned int bIndex, std::list<unsigned int>* visited);

    Dictionary* mDictionary;
    const Board* mBoard;
};
