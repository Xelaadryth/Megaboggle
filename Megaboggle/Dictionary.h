#pragma once

#include <algorithm>
#include <atomic>
#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <vector>

const int LETTER_COUNT = 26;

struct DictionaryNode
{
    DictionaryNode(DictionaryNode* parent, const char value);
    ~DictionaryNode();

    std::atomic_bool mIsDisabled;
    const char mValue;
    bool mIsWord;
    bool mIsFound;
    std::atomic<int> mChildrenCount;
    DictionaryNode* mChildren[LETTER_COUNT];
    DictionaryNode* mParent;
    std::string mWord;
};

class Dictionary
{
public:
    Dictionary(const std::string filename);
    ~Dictionary();

    DictionaryNode* GetRoot();
    bool AddWord(std::string word);
    void OutputResults(const std::string filename);
    static void RemoveWord(DictionaryNode* node);

    unsigned int mMaxDepth = 0;

private:
    //Counts number found and resets dictionary to initial state
    void RecursiveFindFound(DictionaryNode* curNode, std::vector<std::string>& foundWords);
    DictionaryNode* mRoot;
};
