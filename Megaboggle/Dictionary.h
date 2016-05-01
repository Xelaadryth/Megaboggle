#pragma once

#include <atomic>
#include <string>

const int LETTER_COUNT = 26;

struct DictionaryNode
{
    DictionaryNode(DictionaryNode* parent, const char value, unsigned int depth);
    ~DictionaryNode();

    DictionaryNode* mParent;
    const char mValue;
    const unsigned int mDepth;
    std::atomic_bool mIsWord;
    std::atomic_bool mIsDisabled;
    std::atomic<int> mChildrenCount;
    DictionaryNode* mChildren[LETTER_COUNT];
};

class Dictionary
{
public:
    Dictionary(const std::string filename);
    ~Dictionary();

    DictionaryNode* getRoot();
    bool addWord(std::string word);
    static void removeWord(DictionaryNode* node);
    static int charToIndex(char c);

private:
    DictionaryNode* mRoot;
};
