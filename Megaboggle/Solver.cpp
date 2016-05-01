#include "Solver.h"

//Static variables
FILE* Solver::solverOutfile;
std::thread SolverThreadPool::mThreads[NUM_THREADS];
void(*SolverThreadPool::mFnc)(Search *);
//Theoretically there's a race condition where if more than MIN_INT threads decrement this at the same time, it overflows to max int
std::atomic<int> SolverThreadPool::mNumWorkItems;

Search::Search(Dictionary* dictionary, DictionaryNode* dNode, const Board* board, unsigned int bIndex, std::list<unsigned int>* visited) :
    mDictionary(dictionary),
    mDNode(dNode),
    mBoard(board),
    mBIndex(bIndex),
    mVisited(visited)
{
}

SolverThreadPool::SolverThreadPool(void(*fnc)(Search *), int numWorkItems)
{
    SolverThreadPool::mFnc = fnc;
    SolverThreadPool::mNumWorkItems = numWorkItems;
}

void SolverThreadPool::start(Dictionary* dictionary, const Board* board)
{
    for (unsigned int i = 0; i < NUM_THREADS; ++i)
    {
        SolverThreadPool::mThreads[i] = std::thread(SolverThreadPool::startSolverWorker, dictionary, board);
    }
}

void SolverThreadPool::join()
{
    for (unsigned int i = 0; i < NUM_THREADS; ++i)
    {
        SolverThreadPool::mThreads[i].join();
    }
}

void SolverThreadPool::startSolverWorker(Dictionary* dictionary, const Board* board)
{
    DictionaryNode* root = dictionary->getRoot();
    //Re-use this vector so we only have to allocate once
    std::list<unsigned int>* visited = new std::list<unsigned int>();
    Search* search = new Search(dictionary, root, board, 0, visited);

    //We exit when the work queue is empty
    while (true)
    {
        //Pull from the work queue
        int bIndex = --SolverThreadPool::mNumWorkItems;
        if (bIndex < 0) {
            delete search;
            delete visited;
            return;
        }
        //Set up and run the search
        search->mDNode = root;
        search->mBIndex = bIndex;
        SolverThreadPool::mFnc(search);
    }

    delete search;
    delete visited;
}

Solver::Solver(Dictionary* dictionary, const Board* board, const std::string filename) :
    mDictionary(dictionary),
    mBoard(board)
{
    fopen_s(&(Solver::solverOutfile), filename.c_str(), "w");
}

Solver::~Solver()
{
    fclose(Solver::solverOutfile);
}

void Solver::solve()
{
    //Start the thread pool on the work queue
    SolverThreadPool threadPool(Solver::recursiveSearch, mBoard->mWidth * mBoard->mHeight);
    threadPool.start(mDictionary, mBoard);

    threadPool.join();
}

void Solver::recursiveSearch(Search* search)
{
    unsigned int oldBIndex = search->mBIndex;

    //Already using this board node as part of our word
    if (indexVisited(oldBIndex, search->mVisited))
    {
        return;
    }

    DictionaryNode* oldDNode = search->mDNode;
    search->mDNode = search->mDictionary->getChild(oldDNode, search->mBoard->mBoard[oldBIndex]);
    //No dictionary entries remain along this path; either never existed or was disabled
    if (!search->mDNode || search->mDNode->mIsDisabled)
    {
        search->mDNode = oldDNode;
        return;
    }

    //Add the current letter to the word we're building
    search->mVisited->push_back(oldBIndex);

    //Check if we have a word yet
    checkSearch(search);

    unsigned int x = oldBIndex % search->mBoard->mWidth;
    unsigned int y = oldBIndex / search->mBoard->mWidth;
    bool hasLeft = x > 0;
    bool hasRight = x < search->mBoard->mWidth - 1;
    bool hasUp = y > 0;
    bool hasDown = y < search->mBoard->mHeight - 1;

    //Look left
    if (hasLeft) {
        search->mBIndex = oldBIndex - 1;
        recursiveSearch(search);

        //Look up-left
        if (hasUp) {
            search->mBIndex = oldBIndex - 1 - search->mBoard->mWidth;
            recursiveSearch(search);
        }

        //Look down-left
        if (hasDown) {
            search->mBIndex = oldBIndex - 1 + search->mBoard->mWidth;
            recursiveSearch(search);
        }
    }

    //Look right
    if (hasRight) {
        search->mBIndex = oldBIndex + 1;
        recursiveSearch(search);

        //Look up-right
        if (hasUp) {
            search->mBIndex = oldBIndex + 1 - search->mBoard->mWidth;
            recursiveSearch(search);
        }

        //Look down-right
        if (hasDown) {
            search->mBIndex = oldBIndex + 1 + search->mBoard->mWidth;
            recursiveSearch(search);
        }
    }

    //Look up
    if (hasUp) {
        search->mBIndex = oldBIndex - search->mBoard->mWidth;
        recursiveSearch(search);
    }

    //Look down
    if (hasDown) {
        search->mBIndex = oldBIndex + search->mBoard->mWidth;
        recursiveSearch(search);
    }
    
    search->mBIndex = oldBIndex;
    search->mDNode = oldDNode;
    search->mVisited->pop_back();
}

void Solver::checkSearch(Search* search)
{
    //Only one thread is allowed to remove any given word
    bool expected = true;
    if (search->mDNode->mIsWord.compare_exchange_strong(expected, false, std::memory_order_relaxed, std::memory_order_relaxed))
    {
        //Found a word! Remove it from the dictionary
        search->mDictionary->removeWord(search->mDNode);

        //Aggregate and print it
        char* word = new char[search->mDNode->mDepth + 1]();

        unsigned int i = 0;
        for (std::list<unsigned int>::iterator it = search->mVisited->begin(); it != search->mVisited->end(); ++it)
        {
            word[i] = search->mBoard->mBoard[*it];
            ++i;
        }
        fprintf(Solver::solverOutfile, "%s\n", word);

        delete word;
    }
}

inline bool Solver::indexVisited(unsigned int bIndex, std::list<unsigned int>* visited)
{
    std::list<unsigned int>::iterator result = std::find(visited->begin(), visited->end(), bIndex);
    if (result == visited->end())
    {
        return false;
    }
    return true;
}
