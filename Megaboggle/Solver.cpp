#include "Solver.h"

//Static variables
FILE* Solver::solverOutfile;
std::thread SolverThreadPool::mThreads[NUM_THREADS];
void(*SolverThreadPool::mFnc)(Search *);
//Theoretically there's a race condition where more than MIN_INT threads
//decrement at the same time and it overflows to max int
std::atomic<int> SolverThreadPool::mNumWorkItems;

Search::Search(Dictionary* dictionary, DictionaryNode* dNode, const Board* board, unsigned int bIndex, std::vector<unsigned int>* visited) :
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
    std::vector<unsigned int>* visited = new std::vector<unsigned int>();
    Search* search = new Search(dictionary, root, board, 0, visited);

    //We exit when the work queue is empty
    while (true)
    {
        //Pull a row to process from the work queue
        int row = --SolverThreadPool::mNumWorkItems;
        if (row < 0) {
            delete search;
            delete visited;
            return;
        }

        if (BY_ROW) {
            //Solve for every element in that row
            for (unsigned int i = 0; i < board->mWidth; ++i)
            {
                unsigned int bIndex = row * board->mWidth + i;

                //Set up and run the search
                search->mDNode = root;
                search->mBIndex = bIndex;
                SolverThreadPool::mFnc(search);
            }
        }
        else
        {
            //Set up and run the search
            search->mDNode = root;
            search->mBIndex = row;
            SolverThreadPool::mFnc(search);
        }
    }

    delete search;
    delete visited;
}

Solver::Solver(Dictionary* dictionary, const Board* board, const std::string filename) :
    mDictionary(dictionary),
    mBoard(board)
{
    if (BY_ROW) {
        mThreadPool = new SolverThreadPool(Solver::recursiveSearch, board->mHeight);
    }
    else
    {
        mThreadPool = new SolverThreadPool(Solver::recursiveSearch, board->mWidth * board->mHeight);
    }
    fopen_s(&(Solver::solverOutfile), filename.c_str(), "w");
}

Solver::~Solver()
{
    fclose(Solver::solverOutfile);
    delete mThreadPool;
}

void Solver::solve()
{
    //Start the thread pool on the work queue
    mThreadPool->start(mDictionary, mBoard);
    mThreadPool->join();
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
        unsigned int length = search->mVisited->size();
        char* word = new char[length + 1];
        for (unsigned int i = 0; i < length; ++i)
        {
            word[i] = search->mBoard->mBoard[(*search->mVisited)[i]];
        }
        word[length] = '\0';

        fprintf(Solver::solverOutfile, "%s\n", word);

        delete word;
    }
}

inline bool Solver::indexVisited(unsigned int bIndex, std::vector<unsigned int>* visited)
{
    for (unsigned int i = 0; i < visited->size(); ++i)
    {
        if (bIndex == (*visited)[i])
        {
            return true;
        }
    }
    return false;
}
