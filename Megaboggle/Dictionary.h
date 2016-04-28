#pragma once

#include <list>
#include <string>

const int LETTER_COUNT = 26;

struct DictionaryNode
{
	DictionaryNode* mParent;
	const char mValue;
	int mChildrenCount;
	DictionaryNode* mChildren[LETTER_COUNT];

	DictionaryNode(DictionaryNode* parent, const char value);
	~DictionaryNode();
};

class Dictionary
{
public:
	Dictionary(const std::string filePath);
	~Dictionary();

	bool addWord(std::string word);
	void removeChild(const DictionaryNode* child);

private:
	DictionaryNode* mRoot;
};