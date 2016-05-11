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
    std::unique_ptr<DictionaryNode> mChildren[LETTER_COUNT];
    DictionaryNode* mParent;
    std::string mWord;
};

class Dictionary
{
public:
    Dictionary(const std::string filename);
    ~Dictionary();

    DictionaryNode* getRoot();
    bool addWord(std::string word);
    void outputResults(const std::string filename);
    static void removeWord(DictionaryNode* node);

    unsigned int mMaxDepth = 0;

private:
    //Counts number found and resets dictionary to initial state
    void recursiveFindFound(DictionaryNode* curNode, std::vector<std::string>& foundWords);
    std::unique_ptr<DictionaryNode> mRoot;
};
