#pragma once

#include <fstream>
#include <string>
#include "Dictionary.h"

DictionaryNode::DictionaryNode(DictionaryNode* parent, char value) :
	mParent(parent),
	mValue(value),
	mChildrenCount(0),
	mChildren{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} {}

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

	// Add all the words!
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

	// Filter out anything that isn't just letters
	for (int i = 0; i < wordLength; ++i)
	{
		if (word[i] < 'a' || 'z' < word[i])
		{
			return false;
		}
	}

	// Construct our structs one letter at a time
	for (int i = 0; i < wordLength; ++i)
	{
		// TODO: STUFF
	}

	return true;
}

void Dictionary::removeChild(const DictionaryNode* child)
{

}
