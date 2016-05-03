#pragma once

#include <atomic>
#include <fstream>
#include <list>
#include <string>

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

    DictionaryNode* getRoot();
    bool addWord(std::string word);
    void outputResults(const std::string filename);
    static void removeWord(DictionaryNode* node);

private:
    void recursiveFindFound(DictionaryNode* curNode, std::list<std::string>* foundWords);
    DictionaryNode* mRoot;
};
