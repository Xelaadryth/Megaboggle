#pragma once

#include <fstream>
#include <string>
#include "Dictionary.h"

DictionaryNode::DictionaryNode(DictionaryNode* parent, char value) :
	mParent(parent),
	mValue(value),
	mChildrenCount(0),
	mChildren() {}

DictionaryNode::~DictionaryNode()
{
	for (int i = 0; i < LETTER_COUNT; ++i)
	{
		if (mChildren[i])
		{
			delete mChildren[i];
		}
	}
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
			addWord(line);
		}
	}
}

Dictionary::~Dictionary()
{
	delete mRoot;
}

bool Dictionary::addWord(std::string wordString)
{
	size_t wordLength = wordString.length();
	const char* word = wordString.c_str();

	//Filter out anything that isn't just letters
	for (int i = 0; i < wordLength; ++i)
	{
		if (word[i] < 'a' || 'z' < word[i])
		{
			return false;
		}
	}

	DictionaryNode* curNode = mRoot;
	//Construct our structs one letter at a time
	for (int i = 0; i < wordLength; ++i)
	{
		//Convert our char to an integer value so we can use it as an index
		int letterIndex = CharToIndex(word[i]);

		if (!curNode->mChildren[letterIndex])
		{
			curNode->mChildren[letterIndex] = new DictionaryNode(curNode, word[i]);
			curNode->mChildrenCount = curNode->mChildrenCount + 1;
		}
		curNode = curNode->mChildren[letterIndex];
	}

	return true;
}

int Dictionary::CharToIndex(const char c)
{
	return (int)(c - 'a');
}

char Dictionary::IndexToChar(int index)
{
	return (char)(index) + 'a';
}

void Dictionary::removeNode(DictionaryNode* node)
{
	DictionaryNode* parent = node->mParent;
	parent->mChildren[CharToIndex(node->mValue)] = nullptr;
	parent->mChildrenCount = parent->mChildrenCount - 1;

	//If a node no longer has children, then we should remove that one too
	if (parent->mChildrenCount == 0)
	{
		removeNode(parent);
	}
}
