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

                Search* search = new Search(dictionary, Dictionary::getChild(dictionary->getRoot(), board->mBoard[bIndex]), board, bIndex, visited, threadNum);
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
    //Found a word!
    //TODO: Make this work in parallel with fprintf on a global file
    bool expected = true;
    if (search->mDNode->mIsWord.compare_exchange_strong(expected, false))
    {

        //Aggregate and print the word
        char* word = new char[search->mDNode->mDepth + 1]();

        unsigned int i = 0;
        for (std::list<unsigned int>::iterator it = search->mVisited->begin(); it != search->mVisited->end(); ++it)
        {
            word[i] = search->mBoard->mBoard[*it];
            ++i;
        }
        fprintf(mOutfile, "%s\n", word);

        delete word;

        //Remove the word from the dictionary
        search->mDictionary->removeWord(search->mDNode);
    }
}

void Solver::recursiveSearch(Search* search)
{
    unsigned int oldBIndex = search->mBIndex;
    DictionaryNode* oldDNode = search->mDNode;

    search->mVisited->push_back(oldBIndex);

    //Check if we have a word yet
    checkSearch(search);

    //Look left
    unsigned int x = oldBIndex % search->mBoard->mWidth;
    if (x > 0) {
        search->mBIndex = oldBIndex - 1;

        if (indexVisited(search->mBIndex, search->mVisited))
        {
            search->mDNode = search->mDictionary->getChild(search->mDNode, search->mBoard->mBoard[search->mBIndex]);
            if (search->mDNode && !search->mDNode->mIsDisabled)
            {
                recursiveSearch(search);
            }
            search->mDNode = oldDNode;
        }
    }

    //Look right
    if (x < search->mBoard->mWidth - 1) {
        search->mBIndex = oldBIndex + 1;

        if (indexVisited(search->mBIndex, search->mVisited))
        {
            search->mDNode = search->mDictionary->getChild(search->mDNode, search->mBoard->mBoard[search->mBIndex]);
            if (search->mDNode)
            {
                recursiveSearch(search);
            }
            search->mDNode = oldDNode;
        }
    }

    //Look up
    unsigned int y = search->mBIndex / search->mBoard->mWidth;
    if (y > 0) {
        search->mBIndex = oldBIndex - search->mBoard->mWidth;
        if (indexVisited(search->mBIndex, search->mVisited))
        {
            search->mDNode = search->mDictionary->getChild(search->mDNode, search->mBoard->mBoard[search->mBIndex]);
            if (search->mDNode)
            {
                recursiveSearch(search);
            }
            search->mDNode = oldDNode;
        }
    }

    //Look down
    if (y < search->mBoard->mHeight - 1) {
        search->mBIndex = oldBIndex + search->mBoard->mWidth;

        if (indexVisited(search->mBIndex, search->mVisited))
        {
            search->mDNode = search->mDictionary->getChild(search->mDNode, search->mBoard->mBoard[search->mBIndex]);
            if (search->mDNode)
            {
                recursiveSearch(search);
            }
            search->mDNode = oldDNode;
            }
    }

    search->mBIndex = oldBIndex;
    search->mVisited->pop_back();
}

inline bool Solver::indexVisited(unsigned int bIndex, std::list<unsigned int>* visited)
{
    std::list<unsigned int>::iterator result = std::find(visited->begin(), visited->end(), bIndex);
    if (result == visited->end())
    {
        return true;
    }
    return false;
}
