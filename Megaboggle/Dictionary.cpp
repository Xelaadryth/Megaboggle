#pragma once

#include <atomic>
#include <fstream>
#include <string>
#include "Dictionary.h"

#define MIN_WORD_LENGTH 3

DictionaryNode::DictionaryNode(DictionaryNode* parent, char value, unsigned int depth) :
    mParent(parent),
    mValue(value),
    mDepth(depth),
    mIsWord(false),
    mIsDisabled(false),
    mChildrenCount(0),
    mChildren()
{
}

DictionaryNode::~DictionaryNode()
{
    for (unsigned int i = 0; i < LETTER_COUNT; ++i)
    {
        if (mChildren[i])
        {
            delete mChildren[i];
        }
    }
}

DictionaryNode* Dictionary::getRoot()
{
    return mRoot;
}

bool Dictionary::addWord(std::string wordString)
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
        int letterIndex = charToIndex(word[i]);

        //Create the next node if it doesn't already exist
        if (!curNode->mChildren[letterIndex])
        {
            curNode->mChildren[letterIndex] = new DictionaryNode(curNode, word[i], curNode->mDepth + 1);
            ++(curNode->mChildrenCount);
        }
        curNode = curNode->mChildren[letterIndex];
    }

    //The last node must be marked as a word
    curNode->mIsWord = true;

    return true;
}

DictionaryNode* Dictionary::getChild(const DictionaryNode* parent, const char value)
{
    return parent->mChildren[charToIndex(value)];
}

int Dictionary::charToIndex(char c)
{
    return (int)(c - 'a');
}

char Dictionary::indexToChar(int index)
{
    return (char)(index) + 'a';
}

void Dictionary::removeWord(DictionaryNode* node)
{
    //If we are a leaf node, then remove ourselves and decrement our parent's counter
    if (node->mChildrenCount == 0)
    {
        //Multiple people can try to remove this node at the same time, but only one will succeed
        bool expected = false;
        if (node->mIsDisabled.compare_exchange_strong(expected, true))
        {
            //The node is effectively disabled
            node->mIsDisabled = true;

            DictionaryNode* parent = node->mParent;
            --parent->mChildrenCount;

            //If a node no longer has children and isn't a word, then we can try to cascade remove that one too
            if (!parent->mIsWord)
            {
                removeWord(parent);
            }
        }
    }
}

Dictionary::Dictionary(const std::string filename)
{
    mRoot = new DictionaryNode(nullptr, '0', 0);

    //Add all the words!
    std::string line;
    std::ifstream dictionaryFile(filename);
    if (dictionaryFile.is_open())
    {
        while (std::getline(dictionaryFile, line))
        {
            addWord(line);
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
