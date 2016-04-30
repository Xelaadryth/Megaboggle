#include "Solver.h"

Search::Search(Dictionary* dictionary, DictionaryNode* dNode, const Board* board, unsigned int bIndex, std::list<unsigned int>* visited, unsigned int threadNum) :
	mDictionary(dictionary),
	mDNode(dNode),
	mBoard(board),
	mBIndex(bIndex),
	mVisited(visited),
	mThreadNum(threadNum)
{
}

Solver::Solver(Dictionary* dictionary, const Board* board) :
    mDictionary(dictionary),
    mBoard(board) {}

Solver::~Solver()
{
	//delete mAnswers;
}

void Solver::solve()
{
    unsigned int curLetter = 0;
    const DictionaryNode* dRoot = mDictionary->getRoot();

	std::thread mThreads[NUM_THREADS];

	for (unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		mThreads[i] = std::thread(startSearch, mDictionary, mBoard, i);
	}

	for (unsigned int i = 0; i < NUM_THREADS; ++i)
	{
		mThreads[i].join();
	}
}

void Solver::startSearch(Dictionary* dictionary, const Board* board, unsigned int threadNum)
{
	unsigned int width = board->mWidth;

	//Re-use this vector so we only have to allocate once
	std::list<unsigned int>* visited = new std::list<unsigned int>();

	unsigned int distributionIndex = 0;
	for (unsigned int j = 0; j < board->mHeight; ++j)
	{
		for (unsigned int i = 0; i < width; ++i)
		{
			//Distribute search evenly among threads
			if (distributionIndex % NUM_THREADS == threadNum)
			{
				//Get the place on the board to start from
				unsigned int bIndex = j * width + i;

				visited->push_back(bIndex);
				Search* search = new Search(dictionary, Dictionary::getChild(dictionary->getRoot(), board->mBoard[bIndex]), board, bIndex, visited, threadNum);
				recursiveSearch(search);

				//Clear the vector
				visited->clear();
				delete search;
			}
			++distributionIndex;
		}
	}

	delete visited;
}

void Solver::recursiveSearch(Search* search)
{
	//Found a word!
	//TODO: Make this work in parallel
	if (search->mDNode->mIsWord)
	{
		char* word = new char[search->mDNode->mDepth + 1]();

		unsigned int i = 0;
		for (std::list<unsigned int>::iterator it = search->mVisited->begin(); it != search->mVisited->end(); ++it)
		{
			word[i] = search->mBoard->mBoard[*it];
			++i;
		}
		printf("%s\n", word);

		delete word;
		//TODO: Make this allowable
		//search->mDictionary->removeWord(search->mDNode);
		search->mDNode->mIsWord = false;
	}

	//Look left
	unsigned int x = search->mBIndex % search->mBoard->mWidth;
	if (x > 0) {
		legalNext(search, search->mBIndex - 1);
	}

	//Look right
	if (x < search->mBoard->mWidth - 1) {
		legalNext(search, search->mBIndex + 1);
	}

	//Look up
	unsigned int y = search->mBIndex / search->mBoard->mWidth;
	if (y > 0) {
		legalNext(search, search->mBIndex - search->mBoard->mWidth);
	}

	//Look down
	if (y < search->mBoard->mHeight - 1) {
		legalNext(search, search->mBIndex + search->mBoard->mWidth);
	}


}

void Solver::legalNext(Search* search, unsigned int potentialIndex)
{
	DictionaryNode* oldDNode = search->mDNode;
	DictionaryNode* nextDNode = search->mDictionary->getChild(search->mDNode, search->mBoard->mBoard[potentialIndex]);

	unsigned int oldBIndex = search->mBIndex;

	if (nextDNode)
	{
		std::list<unsigned int>::iterator iter = std::find(search->mVisited->begin(), search->mVisited->end(), potentialIndex);
		if (iter == search->mVisited->end())
		{
			search->mVisited->push_back(potentialIndex);
			search->mDNode = nextDNode;
			search->mBIndex = potentialIndex;
			recursiveSearch(search);
			search->mBIndex = oldBIndex;
			search->mVisited->pop_back();
			search->mDNode = oldDNode;
		}
	}
}
