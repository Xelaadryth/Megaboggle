#pragma once

#include <fstream>
#include <string>
#include "Dictionary.h"

#define MIN_WORD_LENGTH 3

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

const DictionaryNode * Dictionary::getRoot()
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
		//Get the index of the next letter
		int letterIndex = charToIndex(word[i]);

		//Create the next node if it doesn't already exist
		if (!curNode->mChildren[letterIndex])
		{
			curNode->mChildren[charToIndex(word[i])] = new DictionaryNode(curNode, word[i]);
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
	node->mIsWord = false;

	//If we are a leaf node, then remove ourselves and decrement our parent's counter
	if (node->mChildrenCount == 0)
	{
		DictionaryNode* parent = node->mParent;
		//TODO: In a parallel implementation, this needs to be atomic
		--(parent->mChildrenCount);
		parent->mChildren[charToIndex(node->mValue)] = nullptr;
		delete node;

		//If a node no longer has children and isn't a word, then we should cascade remove that one too
		if (parent->mChildrenCount == 0 && !parent->mIsWord)
		{
			removeWord(parent);
		}
	}
}
