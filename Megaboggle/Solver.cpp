#include "Solver.h"

//Static variables
std::thread SolverThreadPool::mThreads[NUM_THREADS];
void(*SolverThreadPool::mFnc)(Search *);
//Theoretically there's a race condition where more than MIN_INT threads
//decrement at the same time and it overflows to max int
std::atomic<int> SolverThreadPool::mNumWorkItems;

Search::Search(Dictionary* dictionary, DictionaryNode* dNode, const Board* board, unsigned int bIndex) :
    mDNode(dNode),
    mBIndex(bIndex),
    mBoard(board),
    mDictionary(dictionary)
{
    mVisited = new std::vector<unsigned int>();
}

Search::~Search()
{
    delete mVisited;
}

SolverThreadPool::SolverThreadPool(void(*fnc)(Search *), int numWorkItems)
{
    SolverThreadPool::mFnc = fnc;
    SolverThreadPool::mNumWorkItems.store(numWorkItems - 1);
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
    Search* search = new Search(dictionary, root, board, 0);

    //We exit when the work queue is empty
    while (true)
    {
        //Pull a row to process from the work queue
        int row = SolverThreadPool::mNumWorkItems.fetch_sub(1);
        //We decrement after so we execute on every element from [0, mNumWorkItems - 1]
        if (row < 0) {
            delete search;
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
            //Set up and run the search for an individual element
            search->mDNode = root;
            search->mBIndex = row;
            SolverThreadPool::mFnc(search);
        }
    }

    delete search;
}

Solver::Solver(Dictionary* dictionary, const Board* board) :
    mDictionary(dictionary),
    mBoard(board)
{
    //Chop up work to put in the work queue
    if (BY_ROW) {
        mThreadPool = new SolverThreadPool(Solver::recursiveSearch, board->mHeight);
    }
    else
    {
        mThreadPool = new SolverThreadPool(Solver::recursiveSearch, board->mWidth * board->mHeight);
    }
}

Solver::~Solver()
{
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
    unsigned int currentBIndex = search->mBIndex;

    //Already using this board node as part of our word
    if (indexVisited(currentBIndex, search->mVisited))
    {
        return;
    }

    DictionaryNode* oldDNode = search->mDNode;
    search->mDNode = oldDNode->mChildren[(int)(search->mBoard->mBoard[currentBIndex]) - 'a'];
    //No dictionary entries remain along this path; either never existed or was disabled
    if (!search->mDNode || search->mDNode->mIsDisabled.load())
    {
        search->mDNode = oldDNode;
        return;
    }

    //Check if we found a new word
    int numChildren = search->mDNode->mChildrenCount.load();
    if (search->mDNode->mIsWord && !search->mDNode->mIsFound)
    {
        //Found a word!
        search->mDNode->mIsFound = true;

        //Check if we need to remove it from dictionary
        if (!numChildren)
        {
            //Removing words saves us time later, especially for repetitive boards
            Dictionary::removeWord(search->mDNode);
            search->mDNode = oldDNode;
            return;
        }
    }

    //No need to search farther if the dictionary has no more valid words beneath it
    if (!numChildren)
    {
        search->mDNode = oldDNode;
        return;
    }

    //Add the current board position to our visited list
    search->mVisited->push_back(currentBIndex);

    unsigned int x = currentBIndex % search->mBoard->mWidth;
    unsigned int y = currentBIndex / search->mBoard->mWidth;
    //bool hasLeft = x > 0;
    bool hasRight = x < search->mBoard->mWidth - 1;
    //bool hasUp = y > 0;
    bool hasDown = y < search->mBoard->mHeight - 1;

    //Look left: hasLeft == x > 0 == x since x is an unsigned int
    if (x) {
        search->mBIndex = currentBIndex - 1;
        recursiveSearch(search);

        //Look up-left: hasUp == y > 0 == y since y is an unsigned int
        if (y) {
            search->mBIndex = currentBIndex - 1 - search->mBoard->mWidth;
            recursiveSearch(search);
        }

        //Look down-left
        if (hasDown) {
            search->mBIndex = currentBIndex - 1 + search->mBoard->mWidth;
            recursiveSearch(search);
        }
    }

    //Look right
    if (hasRight) {
        search->mBIndex = currentBIndex + 1;
        recursiveSearch(search);

        //Look up-right
        if (y) {
            search->mBIndex = currentBIndex + 1 - search->mBoard->mWidth;
            recursiveSearch(search);
        }

        //Look down-right
        if (hasDown) {
            search->mBIndex = currentBIndex + 1 + search->mBoard->mWidth;
            recursiveSearch(search);
        }
    }

    //Look up
    if (y) {
        search->mBIndex = currentBIndex - search->mBoard->mWidth;
        recursiveSearch(search);
    }

    //Look down
    if (hasDown) {
        search->mBIndex = currentBIndex + search->mBoard->mWidth;
        recursiveSearch(search);
    }
    
    search->mBIndex = currentBIndex;
    search->mDNode = oldDNode;
    search->mVisited->pop_back();
}

inline bool Solver::indexVisited(unsigned int bIndex, std::vector<unsigned int>* visited)
{
    for (std::vector<unsigned int>::const_iterator iterator = visited->begin(), end = visited->end(); iterator != end; ++iterator)
    {
        if (bIndex == *iterator)
        {
            return true;
        }
    }
    return false;
}
