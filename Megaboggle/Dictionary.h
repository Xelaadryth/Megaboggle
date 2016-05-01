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
    void removeWord(DictionaryNode* node);
    static DictionaryNode* getChild(const DictionaryNode* parent, const char value);

private:
    DictionaryNode* mRoot;

    static int charToIndex(char c);
    static char indexToChar(int index);
};
