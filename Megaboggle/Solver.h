#pragma once

#include "Board.h"
#include "Dictionary.h"
#include <list>
#include <list>
#include <thread>

const int NUM_THREADS = 1;

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
    Solver(Dictionary* dictionary, const Board* board);
	~Solver();

    void solve();

private:
	static void startSearch(Dictionary* dictionary, const Board* board, unsigned int threadNum);
	static void recursiveSearch(Search* search);
	static void legalNext(Search* search, unsigned int potentialIndex);

    Dictionary* mDictionary;
    const Board* mBoard;

};
