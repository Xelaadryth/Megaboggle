#include <fstream>
#include <vector>
#include <string>

#include "Board.h"

Board::Board(const std::string filename)
{
    //Add all the words!
    std::string line;
    std::ifstream boardFile(filename);
    if (boardFile.is_open())
    {
        //Get a 1D list of all the letters in the board and store the height/width
        std::vector<char> charList;

		unsigned int j;
        for (j = 0; std::getline(boardFile, line); ++j)
        {
			size_t lineLength = line.length();

            for (unsigned int lineIndex = 0; lineIndex < lineLength; lineIndex++)
            {
                //Comma-separated means every other character is a comma, newline char not included since we called getline
                if (lineIndex % 2 == 0)
                {
                    charList.push_back(line.at(lineIndex));
                }
            }

            //Record the number of columns after the first row
            if (j == 0)
            {
                mWidth = (unsigned int)charList.size();
            }
        }

		if (j > 0)
		{
			mHeight = j;
		}
		else
		{
			printf("Invalid board.\n");
		}

		mBoard = new char[mHeight * mWidth]();

		for (size_t i = 0, length = charList.size(); i < length; ++i)
		{
			mBoard[i] = charList[i];
		}
    }
    else
    {
        printf("\n\nFailed to open board file: %s\n", filename.c_str());
    }
}

Board::~Board()
{
	delete mBoard;
}
