#pragma once

#include "Dictionary.h"

#define MIN_WORD_LENGTH 3

DictionaryNode::DictionaryNode(DictionaryNode* parent, char value) :
    mValue(value),
    mIsWord(false),
    mIsFound(false),
    mChildrenCount(0),
    mChildren(),
    mParent(parent)
{
    mIsDisabled.store(false);

    if (parent == nullptr)
    {
        mWord = "";
    }
    else
    {
        mWord = parent->mWord + value;
    }
}

DictionaryNode::~DictionaryNode()
{
    for (unsigned int i = 0; i < LETTER_COUNT; ++i)
    {
        delete mChildren[i];
    }
}

DictionaryNode* Dictionary::GetRoot()
{
    return mRoot;
}

bool Dictionary::AddWord(std::string wordString)
{
    size_t wordLength = wordString.length();

    // Filter out any words that are less than 3 letters
    if (wordLength < MIN_WORD_LENGTH)
    {
        return false;
    }

    const char* word = wordString.c_str();

    //Filter out anything that isn't just letters
    for (unsigned int i = 0; i < wordLength; ++i)
    {
        if (word[i] < 'a' || 'z' < word[i])
        {
            return false;
        }
    }

    DictionaryNode* curNode = mRoot;
    //Construct our structs one letter at a time
    for (unsigned int i = 0; i < wordLength; ++i)
    {
        //Get the index of the next letter
        int letterIndex = (int)(word[i] - 'a');

        //Create the next node if it doesn't already exist
        if (!curNode->mChildren[letterIndex])
        {

            curNode->mChildren[letterIndex] = new DictionaryNode(curNode, word[i]);
            curNode->mChildrenCount.fetch_add(1);
        }
        curNode = curNode->mChildren[letterIndex];
    }

    //The last node must be marked as a word
    curNode->mIsWord = true;

    if (wordLength > mMaxDepth)
    {
        mMaxDepth = wordLength;
    }

    return true;
}

void Dictionary::OutputResults(const std::string filename)
{
    std::vector<std::string> foundWords;

    //Get all of the words from the dictionary
    RecursiveFindFound(mRoot, foundWords);

    //Should already be sorted
    //std::sort(foundWords.begin(), foundWords.end());

    FILE* outfile;
    fopen_s(&outfile, filename.c_str(), "w");
    //Print out the number of words and then all of the words alphabetically
    fprintf(outfile, "%d\n", (int)foundWords.size());

    for (std::vector<std::string>::const_iterator iterator = foundWords.begin(), end = foundWords.end(); iterator != end; ++iterator)
    {
        fprintf(outfile, "%s\n", (*iterator).c_str());
    }

    fclose(outfile);
}

//If we are a leaf node, then remove ourselves and decrement our parent's counter
void Dictionary::RemoveWord(DictionaryNode* node)
{
    //Multiple people can try to remove this node at the same time, but only one will succeed
    bool expected = false;
    if (node->mIsDisabled.compare_exchange_strong(expected, true, std::memory_order_relaxed, std::memory_order_relaxed))
    {
        DictionaryNode* parent = node->mParent;

        //fetch_sub is a post-decrement
        if (parent->mChildrenCount.fetch_sub(1) == 1 && !parent->mIsDisabled.load() && (!parent->mIsWord || (parent->mIsWord && parent->mIsFound)))
        {
            //If a node no longer has children and has nothing depending on it, then we can try to cascade remove that one too
            RemoveWord(parent);
        }
    }
}

void Dictionary::RecursiveFindFound(DictionaryNode* curNode, std::vector<std::string>& foundWords)
{
    //Re-enable the dictionary if necessary
    curNode->mIsDisabled.store(false);

    if (curNode->mIsFound)
    {
        foundWords.push_back(curNode->mWord);
        curNode->mIsFound = false;
    }

    int numChildren = 0;
    for (unsigned int i = 0; i < LETTER_COUNT; ++i)
    {
        DictionaryNode* child = curNode->mChildren[i];
        if (child)
        {
            ++numChildren;
            RecursiveFindFound(child, foundWords);
        }
    }

    curNode->mChildrenCount.store(numChildren);
}

Dictionary::Dictionary(const std::string filename)
{
    mRoot = new DictionaryNode(nullptr, '0');

    //Add all the words!
    std::string line;
    std::ifstream dictionaryFile(filename);
    if (dictionaryFile.is_open())
    {
        while (std::getline(dictionaryFile, line))
        {
            AddWord(line);
        }
    }
    else
    {
        printf("\n\nFailed to open dictionary file: %s\n", filename.c_str());
    }
}

Dictionary::~Dictionary()
{
    delete mRoot;
}
