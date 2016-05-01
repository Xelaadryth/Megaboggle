#include "Solver.h"

//TODO: Better place for this
FILE* mOutfile;

Search::Search(Dictionary* dictionary, DictionaryNode* dNode, const Board* board, unsigned int bIndex, std::list<unsigned int>* visited, unsigned int threadNum) :
    mDictionary(dictionary),
    mDNode(dNode),
    mBoard(board),
    mBIndex(bIndex),
    mVisited(visited),
    mThreadNum(threadNum)
{
}

Solver::Solver(Dictionary* dictionary, const Board* board, const std::string filename) :
    mDictionary(dictionary),
    mBoard(board)
{
    fopen_s(&mOutfile, filename.c_str(), "w");
}

Solver::~Solver()
{
    fclose(mOutfile);
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

                Search* search = new Search(dictionary, dictionary->getRoot(), board, bIndex, visited, threadNum);
                recursiveSearch(search);

                delete search;
            }
            ++distributionIndex;
        }
    }

    delete visited;
}

void Solver::checkSearch(Search* search)
{
    //Only one thread is allowed to remove any given word
    bool expected = true;
    if (search->mDNode->mIsWord.compare_exchange_strong(expected, false))
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
        fprintf(mOutfile, "%s\n", word);

        delete word;
    }
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

inline bool Solver::indexVisited(unsigned int bIndex, std::list<unsigned int>* visited)
{
    std::list<unsigned int>::iterator result = std::find(visited->begin(), visited->end(), bIndex);
    if (result == visited->end())
    {
        return false;
    }
    return true;
}
